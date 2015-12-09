class RegAliases(object):
  def __init__(self, *Aliases):
    self.Aliases = list(Aliases)

  def __str__(self):
    return 'REGLIST{AliasCount}(RegARM32, {Aliases})'.format(
      AliasCount=len(self.Aliases), Aliases=', '.join(self.Aliases))

def _ArgumentNames(Method):
  import inspect
  return (ArgName for ArgName in inspect.getargspec(Method).args
      if ArgName != 'self')

class RegFeatures(object):
  def __init__(self, IsScratch=0, IsPreserved=0, IsStackPtr=0, IsFramePtr=0,
               IsInt=0, IsI64Pair=0, IsFP32=0, IsFP64=0, IsVec128=0,
               Aliases=None):
    assert not (IsInt and IsI64Pair)
    assert not (IsFP32 and IsFP64)
    assert not (IsFP32 and IsVec128)
    assert not (IsFP64 and IsVec128)
    assert not ((IsInt or IsI64Pair) and (IsFP32 or IsFP64 or IsVec128))
    assert (not IsFramePtr) or IsInt
    assert (not IsStackPtr) or not(
                IsInt or IsI64Pair or IsFP32 or IsFP64 or IsVec128)
    assert not (IsScratch and IsPreserved)
    self.Features = [x for x in _ArgumentNames(self.__init__)]
    self.FeaturesDict = {}
    for Feature in self.Features:
      self.FeaturesDict[Feature] = locals()[Feature]

  def __str__(self):
    return '%s' % (', '.join(str(self.FeaturesDict[Feature]) for
                                 Feature in self.Features))

  def Aliases(self):
    return self.FeaturesDict['Aliases']

  def LivesInGPR(self):
    return (any(self.FeaturesDict[IntFeature] for IntFeature in (
                   'IsInt', 'IsI64Pair', 'IsStackPtr', 'IsFramePtr')) or
            not self.LivesInVFP())

  def LivesInVFP(self):
    return any(self.FeaturesDict[FpFeature] for FpFeature in (
                   'IsFP32', 'IsFP64', 'IsVec128'))

class Reg(object):
  def __init__(self, Name, Encode, **Features):
    self.Name = Name
    self.Encode = Encode
    self.Features = RegFeatures(**Features)

  def __str__(self):
    return 'Reg_{Name}, {Encode}, {Features}'.format(Name=self.Name,
      Encode=self.Encode, Features=self.Features)

  def IsAnAliasOf(self, Other):
    return self.Name in self.Features.Aliases().Aliases

# Note: The following tables break the usual 80-col on purpose -- it is easier
# to read the register tables if each register entry is contained on a single
# line.
GPRs = [
  Reg( 'r0',  0,   IsScratch=1, IsInt=1,               Aliases=RegAliases( 'r0',  'r0r1')),
  Reg( 'r1',  1,   IsScratch=1, IsInt=1,               Aliases=RegAliases( 'r1',  'r0r1')),
  Reg( 'r2',  2,   IsScratch=1, IsInt=1,               Aliases=RegAliases( 'r2',  'r2r3')),
  Reg( 'r3',  3,   IsScratch=1, IsInt=1,               Aliases=RegAliases( 'r3',  'r2r3')),
  Reg( 'r4',  4, IsPreserved=1, IsInt=1,               Aliases=RegAliases( 'r4',  'r4r5')),
  Reg( 'r5',  5, IsPreserved=1, IsInt=1,               Aliases=RegAliases( 'r5',  'r4r5')),
  Reg( 'r6',  6, IsPreserved=1, IsInt=1,               Aliases=RegAliases( 'r6',  'r6r7')),
  Reg( 'r7',  7, IsPreserved=1, IsInt=1,               Aliases=RegAliases( 'r7',  'r6r7')),
  Reg( 'r8',  8, IsPreserved=1, IsInt=1,               Aliases=RegAliases( 'r8',  'r8r9')),
  Reg( 'r9',  9, IsPreserved=1, IsInt=0,               Aliases=RegAliases( 'r9',  'r8r9')),
  Reg('r10', 10, IsPreserved=1, IsInt=1,               Aliases=RegAliases('r10', 'r10fp')),
  Reg( 'fp', 11, IsPreserved=1, IsInt=1, IsFramePtr=1, Aliases=RegAliases( 'fp', 'r10fp')),
  Reg( 'ip', 12,   IsScratch=1, IsInt=1,               Aliases=RegAliases( 'ip')),
  Reg( 'sp', 13,   IsScratch=0, IsInt=0, IsStackPtr=1, Aliases=RegAliases( 'sp')),
  Reg( 'lr', 14,   IsScratch=0, IsInt=0,               Aliases=RegAliases( 'lr')),
  Reg( 'pc', 15,   IsScratch=0, IsInt=0,               Aliases=RegAliases( 'pc')),
]

I64Pairs = [
  Reg( 'r0r1',  0,   IsScratch=1, IsI64Pair=1, Aliases=RegAliases( 'r0r1',  'r0', 'r1')),
  Reg( 'r2r3',  2,   IsScratch=1, IsI64Pair=1, Aliases=RegAliases( 'r2r3',  'r2', 'r3')),
  Reg( 'r4r5',  4, IsPreserved=1, IsI64Pair=1, Aliases=RegAliases( 'r4r5',  'r4', 'r5')),
  Reg( 'r6r7',  6, IsPreserved=1, IsI64Pair=1, Aliases=RegAliases( 'r6r7',  'r6', 'r7')),
  Reg( 'r8r9',  8, IsPreserved=1, IsI64Pair=0, Aliases=RegAliases( 'r8r9',  'r8', 'r9')),
  Reg('r10fp', 10, IsPreserved=1, IsI64Pair=1, Aliases=RegAliases('r10fp', 'r10', 'fp')),
]

FP32 = [
  Reg( 's0',  0,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's0', 'd0' , 'q0')),
  Reg( 's1',  1,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's1', 'd0' , 'q0')),
  Reg( 's2',  2,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's2', 'd1' , 'q0')),
  Reg( 's3',  3,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's3', 'd1' , 'q0')),
  Reg( 's4',  4,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's4', 'd2' , 'q1')),
  Reg( 's5',  5,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's5', 'd2' , 'q1')),
  Reg( 's6',  6,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's6', 'd3' , 'q1')),
  Reg( 's7',  7,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's7', 'd3' , 'q1')),
  Reg( 's8',  8,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's8', 'd4' , 'q2')),
  Reg( 's9',  9,   IsScratch=1, IsFP32=1, Aliases=RegAliases( 's9', 'd4' , 'q2')),
  Reg('s10', 10,   IsScratch=1, IsFP32=1, Aliases=RegAliases('s10', 'd5' , 'q2')),
  Reg('s11', 11,   IsScratch=1, IsFP32=1, Aliases=RegAliases('s11', 'd5' , 'q2')),
  Reg('s12', 12,   IsScratch=1, IsFP32=1, Aliases=RegAliases('s12', 'd6' , 'q3')),
  Reg('s13', 13,   IsScratch=1, IsFP32=1, Aliases=RegAliases('s13', 'd6' , 'q3')),
  Reg('s14', 14,   IsScratch=1, IsFP32=1, Aliases=RegAliases('s14', 'd7' , 'q3')),
  Reg('s15', 15,   IsScratch=1, IsFP32=1, Aliases=RegAliases('s15', 'd7' , 'q3')),
  Reg('s16', 16, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s16', 'd8' , 'q4')),
  Reg('s17', 17, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s17', 'd8' , 'q4')),
  Reg('s18', 18, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s18', 'd9' , 'q4')),
  Reg('s19', 19, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s19', 'd9' , 'q4')),
  Reg('s20', 20, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s20', 'd10', 'q5')),
  Reg('s21', 21, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s21', 'd10', 'q5')),
  Reg('s22', 22, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s22', 'd11', 'q5')),
  Reg('s23', 23, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s23', 'd11', 'q5')),
  Reg('s24', 24, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s24', 'd12', 'q6')),
  Reg('s25', 25, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s25', 'd12', 'q6')),
  Reg('s26', 26, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s26', 'd13', 'q6')),
  Reg('s27', 27, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s27', 'd13', 'q6')),
  Reg('s28', 28, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s28', 'd14', 'q7')),
  Reg('s29', 29, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s29', 'd14', 'q7')),
  Reg('s30', 30, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s30', 'd15', 'q7')),
  Reg('s31', 31, IsPreserved=1, IsFP32=1, Aliases=RegAliases('s31', 'd14', 'q7')),
]

FP64 = [
  Reg( 'd0',  0,   IsScratch=1, IsFP64=1, Aliases=RegAliases( 'd0',  'q0',  's0',  's1')),
  Reg( 'd1',  1,   IsScratch=1, IsFP64=1, Aliases=RegAliases( 'd1',  'q0',  's2',  's3')),
  Reg( 'd2',  2,   IsScratch=1, IsFP64=1, Aliases=RegAliases( 'd2',  'q1',  's4',  's5')),
  Reg( 'd3',  3,   IsScratch=1, IsFP64=1, Aliases=RegAliases( 'd3',  'q1',  's6',  's7')),
  Reg( 'd4',  4,   IsScratch=1, IsFP64=1, Aliases=RegAliases( 'd4',  'q2',  's8',  's9')),
  Reg( 'd5',  5,   IsScratch=1, IsFP64=1, Aliases=RegAliases( 'd5',  'q2', 's10', 's11')),
  Reg( 'd6',  6,   IsScratch=1, IsFP64=1, Aliases=RegAliases( 'd6',  'q3', 's12', 's13')),
  Reg( 'd7',  7,   IsScratch=1, IsFP64=1, Aliases=RegAliases( 'd7',  'q3', 's14', 's15')),
  Reg( 'd8',  8, IsPreserved=1, IsFP64=1, Aliases=RegAliases( 'd8',  'q4', 's16', 's17')),
  Reg( 'd9',  9, IsPreserved=1, IsFP64=1, Aliases=RegAliases( 'd9',  'q4', 's18', 's19')),
  Reg('d10', 10, IsPreserved=1, IsFP64=1, Aliases=RegAliases('d10',  'q5', 's20', 's21')),
  Reg('d11', 11, IsPreserved=1, IsFP64=1, Aliases=RegAliases('d11',  'q5', 's22', 's24')),
  Reg('d12', 12, IsPreserved=1, IsFP64=1, Aliases=RegAliases('d12',  'q6', 's24', 's25')),
  Reg('d13', 13, IsPreserved=1, IsFP64=1, Aliases=RegAliases('d13',  'q6', 's26', 's27')),
  Reg('d14', 14, IsPreserved=1, IsFP64=1, Aliases=RegAliases('d14',  'q7', 's28', 's28')),
  Reg('d15', 15, IsPreserved=1, IsFP64=1, Aliases=RegAliases('d15',  'q7', 's30', 's31')),
  Reg('d16', 16,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d16',  'q8')),
  Reg('d17', 17,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d17',  'q8')),
  Reg('d18', 18,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d18',  'q9')),
  Reg('d19', 19,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d19',  'q9')),
  Reg('d20', 20,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d20', 'q10')),
  Reg('d21', 21,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d21', 'q10')),
  Reg('d22', 22,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d22', 'q11')),
  Reg('d23', 23,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d23', 'q11')),
  Reg('d24', 24,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d24', 'q12')),
  Reg('d25', 25,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d25', 'q12')),
  Reg('d26', 26,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d26', 'q13')),
  Reg('d27', 27,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d27', 'q13')),
  Reg('d28', 28,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d28', 'q14')),
  Reg('d29', 29,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d29', 'q14')),
  Reg('d30', 30,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d30', 'q15')),
  Reg('d31', 31,   IsScratch=1, IsFP64=1, Aliases=RegAliases('d31', 'q15')),
]

Vec128 = [
  Reg( 'q0',  0,   IsScratch=1, IsVec128=1, Aliases=RegAliases( 'q0',  'd0',  'd1',  's0',  's1',  's2',  's3')),
  Reg( 'q1',  1,   IsScratch=1, IsVec128=1, Aliases=RegAliases( 'q1',  'd2',  'd3',  's4',  's5',  's6',  's7')),
  Reg( 'q2',  2,   IsScratch=1, IsVec128=1, Aliases=RegAliases( 'q2',  'd4',  'd5',  's8',  's9', 's10', 's11')),
  Reg( 'q3',  3,   IsScratch=1, IsVec128=1, Aliases=RegAliases( 'q3',  'd6',  'd7', 's12', 's13', 's14', 's15')),
  Reg( 'q4',  4, IsPreserved=1, IsVec128=1, Aliases=RegAliases( 'q4',  'd8',  'd9', 's16', 's17', 's18', 's19')),
  Reg( 'q5',  5, IsPreserved=1, IsVec128=1, Aliases=RegAliases( 'q5', 'd10', 'd11', 's20', 's21', 's22', 's23')),
  Reg( 'q6',  6, IsPreserved=1, IsVec128=1, Aliases=RegAliases( 'q6', 'd12', 'd13', 's24', 's25', 's26', 's27')),
  Reg( 'q7',  7, IsPreserved=1, IsVec128=1, Aliases=RegAliases( 'q7', 'd14', 'd15', 's28', 's29', 's30', 's31')),
  Reg( 'q8',  8,   IsScratch=1, IsVec128=1, Aliases=RegAliases( 'q8', 'd16', 'd17')),
  Reg( 'q9',  9,   IsScratch=1, IsVec128=1, Aliases=RegAliases( 'q9', 'd18', 'd19')),
  Reg('q10', 10,   IsScratch=1, IsVec128=1, Aliases=RegAliases('q10', 'd20', 'd21')),
  Reg('q11', 11,   IsScratch=1, IsVec128=1, Aliases=RegAliases('q11', 'd22', 'd23')),
  Reg('q12', 12,   IsScratch=1, IsVec128=1, Aliases=RegAliases('q12', 'd24', 'd25')),
  Reg('q13', 13,   IsScratch=1, IsVec128=1, Aliases=RegAliases('q13', 'd26', 'd27')),
  Reg('q14', 14,   IsScratch=1, IsVec128=1, Aliases=RegAliases('q14', 'd28', 'd29')),
  Reg('q15', 15,   IsScratch=1, IsVec128=1, Aliases=RegAliases('q15', 'd30', 'd31')),
]

def _reverse(x):
  return sorted(x, key=lambda x: x.Encode, reverse=True)
RegClasses = [GPRs, I64Pairs, FP32, _reverse(FP64), _reverse(Vec128)]

AllRegs = {}
for RegClass in RegClasses:
  for Reg in RegClass:
    assert Reg.Name not in AllRegs
    AllRegs[Reg.Name] = Reg

for RegClass in RegClasses:
  for Reg in RegClass:
    for Alias in AllRegs[Reg.Name].Features.Aliases().Aliases:
      assert AllRegs[Alias].IsAnAliasOf(Reg), '%s VS %s' % (Reg, AllRegs[Alias])
      assert AllRegs[Alias].IsAnAliasOf(Reg), '%s VS %s' % (Reg, AllRegs[Alias])
      assert (AllRegs[Alias].Features.LivesInGPR() ==
                 Reg.Features.LivesInGPR()), '%s VS %s' % (Reg, AllRegs[Alias])
      assert (AllRegs[Alias].Features.LivesInVFP() ==
                 Reg.Features.LivesInVFP()), '%s VS %s' % (Reg, AllRegs[Alias])

for RegClass in RegClasses:
  for Reg in RegClass:
    print 'X({Reg})'.format(Reg=Reg)
  print
