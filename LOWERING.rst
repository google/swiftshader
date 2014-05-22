Target-specific lowering in ICE
===============================

This document discusses several issues around generating target-specific ICE
instructions from high-level ICE instructions.

Meeting register address mode constraints
-----------------------------------------

Target-specific instructions often require specific operands to be in physical
registers.  Sometimes one specific register is required, but usually any
register in a particular register class will suffice, and that register class is
defined by the instruction/operand type.

The challenge is that ``Variable`` represents an operand that is either a stack
location in the current frame, or a physical register.  Register allocation
happens after target-specific lowering, so during lowering we generally don't
know whether a ``Variable`` operand will meet a target instruction's physical
register requirement.

To this end, ICE allows certain hints/directives:

    * ``Variable::setWeightInfinite()`` forces a ``Variable`` to get some
      physical register (without specifying which particular one) from a
      register class.

    * ``Variable::setRegNum()`` forces a ``Variable`` to be assigned a specific
      physical register.

    * ``Variable::setPreferredRegister()`` registers a preference for a physical
      register based on another ``Variable``'s physical register assignment.

These hints/directives are described below in more detail.  In most cases,
though, they don't need to be explicity used, as the routines that create
lowered instructions have reasonable defaults and simple options that control
these hints/directives.

The recommended ICE lowering strategy is to generate extra assignment
instructions involving extra ``Variable`` temporaries, using the
hints/directives to force suitable register assignments for the temporaries, and
then let the global register allocator clean things up.

Note: There is a spectrum of *implementation complexity* versus *translation
speed* versus *code quality*.  This recommended strategy picks a point on the
spectrum representing very low complexity ("splat-isel"), pretty good code
quality in terms of frame size and register shuffling/spilling, but perhaps not
the fastest translation speed since extra instructions and operands are created
up front and cleaned up at the end.

Ensuring some physical register
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The x86 instruction::

    mov dst, src

needs at least one of its operands in a physical register (ignoring the case
where ``src`` is a constant).  This can be done as follows::

    mov reg, src
    mov dst, reg

so long as ``reg`` is guaranteed to have a physical register assignment.  The
low-level lowering code that accomplishes this looks something like::

    Variable *Reg;
    Reg = Func->makeVariable(Dst->getType());
    Reg->setWeightInfinite();
    NewInst = InstX8632Mov::create(Func, Reg, Src);
    NewInst = InstX8632Mov::create(Func, Dst, Reg);

``Cfg::makeVariable()`` generates a new temporary, and
``Variable::setWeightInfinite()`` gives it infinite weight for the purpose of
register allocation, thus guaranteeing it a physical register.

The ``_mov(Dest, Src)`` method in the ``TargetX8632`` class is sufficiently
powerful to handle these details in most situations.  Its ``Dest`` argument is
an in/out parameter.  If its input value is ``NULL``, then a new temporary
variable is created, its type is set to the same type as the ``Src`` operand, it
is given infinite register weight, and the new ``Variable`` is returned through
the in/out parameter.  (This is in addition to the new temporary being the dest
operand of the ``mov`` instruction.)  The simpler version of the above example
is::

    Variable *Reg = NULL;
    _mov(Reg, Src);
    _mov(Dst, Reg);

Preferring another ``Variable``'s physical register
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

One problem with this example is that the register allocator usually just
assigns the first available register to a live range.  If this instruction ends
the live range of ``src``, this may lead to code like the following::

    mov reg:eax, src:esi
    mov dst:edi, reg:eax

Since the first instruction happens to end the live range of ``src:esi``, it
would be better to assign ``esi`` to ``reg``::

    mov reg:esi, src:esi
    mov dst:edi, reg:esi

The first instruction, ``mov esi, esi``, is a redundant assignment and will
ultimately be elided, leaving just ``mov edi, esi``.

We can tell the register allocator to prefer the register assigned to a
different ``Variable``, using ``Variable::setPreferredRegister()``::

    Variable *Reg;
    Reg = Func->makeVariable(Dst->getType());
    Reg->setWeightInfinite();
    Reg->setPreferredRegister(Src);
    NewInst = InstX8632Mov::create(Func, Reg, Src);
    NewInst = InstX8632Mov::create(Func, Dst, Reg);

Or more simply::

    Variable *Reg = NULL;
    _mov(Reg, Src);
    _mov(Dst, Reg);
    Reg->setPreferredRegister(llvm::dyn_cast<Variable>(Src));

The usefulness of ``setPreferredRegister()`` is tied into the implementation of
the register allocator.  ICE uses linear-scan register allocation, which sorts
live ranges by starting point and assigns registers in that order.  Using
``B->setPreferredRegister(A)`` only helps when ``A`` has already been assigned a
register by the time ``B`` is being considered.  For an assignment ``B=A``, this
is usually a safe assumption because ``B``'s live range begins at this
instruction but ``A``'s live range must have started earlier.  (There may be
exceptions for variables that are no longer in SSA form.)  But
``A->setPreferredRegister(B)`` is unlikely to help unless ``B`` has been
precolored.  In summary, generally the best practice is to use a pattern like::

    NewInst = InstX8632Mov::create(Func, Dst, Src);
    Dst->setPreferredRegister(Src);
    //Src->setPreferredRegister(Dst); -- unlikely to have any effect

Ensuring a specific physical register
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some instructions require operands in specific physical registers, or produce
results in specific physical registers.  For example, the 32-bit ``ret``
instruction needs its operand in ``eax``.  This can be done with
``Variable::setRegNum()``::

    Variable *Reg;
    Reg = Func->makeVariable(Src->getType());
    Reg->setWeightInfinite();
    Reg->setRegNum(Reg_eax);
    NewInst = InstX8632Mov::create(Func, Reg, Src);
    NewInst = InstX8632Ret::create(Func, Reg);

Precoloring with ``Variable::setRegNum()`` effectively gives it infinite weight
for register allocation, so the call to ``Variable::setWeightInfinite()`` is
technically unnecessary, but perhaps documents the intention a bit more
strongly.

The ``_mov(Dest, Src, RegNum)`` method in the ``TargetX8632`` class has an
optional ``RegNum`` argument to force a specific register assignment when the
input ``Dest`` is ``NULL``.  As described above, passing in ``Dest=NULL`` causes
a new temporary variable to be created with infinite register weight, and in
addition the specific register is chosen.  The simpler version of the above
example is::

    Variable *Reg = NULL;
    _mov(Reg, Src, Reg_eax);
    _ret(Reg);

Disabling live-range interference
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Another problem with the "``mov reg,src; mov dst,reg``" example happens when
the instructions do *not* end the live range of ``src``.  In this case, the live
ranges of ``reg`` and ``src`` interfere, so they can't get the same physical
register despite the explicit preference.  However, ``reg`` is meant to be an
alias of ``src`` so they needn't be considered to interfere with each other.
This can be expressed via the second (bool) argument of
``setPreferredRegister()``::

    Variable *Reg;
    Reg = Func->makeVariable(Dst->getType());
    Reg->setWeightInfinite();
    Reg->setPreferredRegister(Src, true);
    NewInst = InstX8632Mov::create(Func, Reg, Src);
    NewInst = InstX8632Mov::create(Func, Dst, Reg);

This should be used with caution and probably only for these short-live-range
temporaries, otherwise the classic "lost copy" or "lost swap" problem may be
encountered.

Instructions with register side effects
---------------------------------------

Some instructions produce unwanted results in other registers, or otherwise kill
preexisting values in other registers.  For example, a ``call`` kills the
scratch registers.  Also, the x86-32 ``idiv`` instruction produces the quotient
in ``eax`` and the remainder in ``edx``, but generally only one of those is
needed in the lowering.  It's important that the register allocator doesn't
allocate that register to a live range that spans the instruction.

ICE provides the ``InstFakeKill`` pseudo-instruction to mark such register
kills.  For each of the instruction's source variables, a fake trivial live
range is created that begins and ends in that instruction.  The ``InstFakeKill``
instruction is inserted after the ``call`` instruction.  For example::

    CallInst = InstX8632Call::create(Func, ... );
    VarList KilledRegs;
    KilledRegs.push_back(eax);
    KilledRegs.push_back(ecx);
    KilledRegs.push_back(edx);
    NewInst = InstFakeKill::create(Func, KilledRegs, CallInst);

The last argument to the ``InstFakeKill`` constructor links it to the previous
call instruction, such that if its linked instruction is dead-code eliminated,
the ``InstFakeKill`` instruction is eliminated as well.

The killed register arguments need to be assigned a physical register via
``Variable::setRegNum()`` for this to be effective.  To avoid a massive
proliferation of ``Variable`` temporaries, the ``TargetLowering`` object caches
one precolored ``Variable`` for each physical register::

    CallInst = InstX8632Call::create(Func, ... );
    VarList KilledRegs;
    Variable *eax = Func->getTarget()->getPhysicalRegister(Reg_eax);
    Variable *ecx = Func->getTarget()->getPhysicalRegister(Reg_ecx);
    Variable *edx = Func->getTarget()->getPhysicalRegister(Reg_edx);
    KilledRegs.push_back(eax);
    KilledRegs.push_back(ecx);
    KilledRegs.push_back(edx);
    NewInst = InstFakeKill::create(Func, KilledRegs, CallInst);

On first glance, it may seem unnecessary to explicitly kill the register that
returns the ``call`` return value.  However, if for some reason the ``call``
result ends up being unused, dead-code elimination could remove dead assignments
and incorrectly expose the return value register to a register allocation
assignment spanning the call, which would be incorrect.

Instructions producing multiple values
--------------------------------------

ICE instructions allow at most one destination ``Variable``.  Some machine
instructions produce more than one usable result.  For example, the x86-32
``call`` ABI returns a 64-bit integer result in the ``edx:eax`` register pair.
Also, x86-32 has a version of the ``imul`` instruction that produces a 64-bit
result in the ``edx:eax`` register pair.

To support multi-dest instructions, ICE provides the ``InstFakeDef``
pseudo-instruction, whose destination can be precolored to the appropriate
physical register.  For example, a ``call`` returning a 64-bit result in
``edx:eax``::

    CallInst = InstX8632Call::create(Func, RegLow, ... );
    ...
    NewInst = InstFakeKill::create(Func, KilledRegs, CallInst);
    Variable *RegHigh = Func->makeVariable(IceType_i32);
    RegHigh->setRegNum(Reg_edx);
    NewInst = InstFakeDef::create(Func, RegHigh);

``RegHigh`` is then assigned into the desired ``Variable``.  If that assignment
ends up being dead-code eliminated, the ``InstFakeDef`` instruction may be
eliminated as well.

Preventing dead-code elimination
--------------------------------

ICE instructions with a non-NULL ``Dest`` are subject to dead-code elimination.
However, some instructions must not be eliminated in order to preserve side
effects.  This applies to most function calls, volatile loads, and loads and
integer divisions where the underlying language and runtime are relying on
hardware exception handling.

ICE facilitates this with the ``InstFakeUse`` pseudo-instruction.  This forces a
use of its source ``Variable`` to keep that variable's definition alive.  Since
the ``InstFakeUse`` instruction has no ``Dest``, it will not be eliminated.

Here is the full example of the x86-32 ``call`` returning a 32-bit integer
result::

    Variable *Reg = Func->makeVariable(IceType_i32);
    Reg->setRegNum(Reg_eax);
    CallInst = InstX8632Call::create(Func, Reg, ... );
    VarList KilledRegs;
    Variable *eax = Func->getTarget()->getPhysicalRegister(Reg_eax);
    Variable *ecx = Func->getTarget()->getPhysicalRegister(Reg_ecx);
    Variable *edx = Func->getTarget()->getPhysicalRegister(Reg_edx);
    KilledRegs.push_back(eax);
    KilledRegs.push_back(ecx);
    KilledRegs.push_back(edx);
    NewInst = InstFakeKill::create(Func, KilledRegs, CallInst);
    NewInst = InstFakeUse::create(Func, Reg);
    NewInst = InstX8632Mov::create(Func, Result, Reg);

Without the ``InstFakeUse``, the entire call sequence could be dead-code
eliminated if its result were unused.

One more note on this topic.  These tools can be used to allow a multi-dest
instruction to be dead-code eliminated only when none of its results is live.
The key is to use the optional source parameter of the ``InstFakeDef``
instruction.  Using pseudocode::

    t1:eax = call foo(arg1, ...)
    InstFakeKill(eax, ecx, edx)
    t2:edx = InstFakeDef(t1)
    v_result_low = t1
    v_result_high = t2

If ``v_result_high`` is live but ``v_result_low`` is dead, adding ``t1`` as an
argument to ``InstFakeDef`` suffices to keep the ``call`` instruction live.
