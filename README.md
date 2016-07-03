SwiftShader
==========

Introduction
-----------------

SwiftShader is a high-performance CPU-based implementation of the OpenGL ES and Direct3D 9 graphics APIs<sup>1</sup><sup>2</sup>. Its goal is to provide hardware independence for advanced 3D graphics.

Usage
--------

The SwiftShader libraries act as drop-in replacements for graphics drivers.

On Windows, most applications can be made to use SwiftShader's DLLs by placing them in the same folder as the executable. On Linux, the LD\_LIBRARY\_PATH environment variable or -rpath linker option can be used to direct applications to search for shared libraries in the indicated directory first.

Contributing
-----------------

See [CONTRIBUTING.txt](CONTRIBUTING.txt) for important contributing requirements.

The canonical repository for SwiftShader is hosted at:
https://swiftshader.googlesource.com/SwiftShader

All changes must be reviewed and approved in the [Gerrit](https://www.gerritcodereview.com/) review tool at:
https://swiftshader-review.googlesource.com

Authenticate your account here:
https://swiftshader-review.googlesource.com/new-password

All changes require a [Change-ID](https://gerrit-review.googlesource.com/Documentation/user-changeid.html) tag in the commit message. A commit hook may be used to add this tag automatically, and can be found at:
https://gerrit-review.googlesource.com/tools/hooks/commit-msg. To clone the repository and install the commit hook in one go: 

    git clone https://swiftshader.googlesource.com/SwiftShader && (cd SwiftShader && curl -Lo `git rev-parse --git-dir`/hooks/commit-msg https://gerrit-review.googlesource.com/tools/hooks/commit-msg ; chmod +x `git rev-parse --git-dir`/hooks/commit-msg)

Changes are uploaded to Gerrit by performing:

    git push origin HEAD:refs/for/master

Documentation
-------------

See [docs/Index.md](docs/Index.md).

Contact
-----------

Public mailing list: swiftshader@googlegroups.com

Bug tracker: https://bugs.chromium.org/p/swiftshader

License
----------

The SwiftShader project is licensed under the Apache License Version 2.0. You can find a copy of it in [LICENSE.txt](LICENSE.txt).

Files in the third_party folder are subject to their respective license.

Authors and Contributors
-----------------------------------

The legal authors for copyright purposes are listed in [AUTHORS.txt](AUTHORS.txt).

[CONTRIBUTORS.txt](CONTRIBUTORS.txt) contains a list of names of individuals who have contributed to SwiftShader. If you're not on the list, but you've signed the [Google CLA](https://cla.developers.google.com/clas) and have contributed more than a formatting change, feel free to request to be added.

Disclaimer
---------------

1. Trademarks are the property of their respective owners.
2. We do not claim official conformance with any graphics APIs at this moment.
3. This is not an official Google product.