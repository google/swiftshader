This repository contains the SwiftShader source code. It was licensed from
TransGaming Inc. for use in conjunction with Google products, and should be
treated as internal closed source.

SwiftShader is a pure software implementation of OpenGL and Direct3D.
It is used by Chrome for WebGL support with a blacklisted GPU, and by Looking
Glass for running Windows applications on a VM without GPU support for
streaming to Chromebooks.

Watermark free versions of the SwiftShader builds should not be deployed on
consumer systems! The watermark allows people to evaluate SwiftShader and
license it from TransGaming if it suits their needs. Chrome has SwiftShader
DLLs which are installed on consumer systems, but Chrome issues a license key
to disable the watermark, so that with any other application the logo pops up.

                                                              - capn@google.com