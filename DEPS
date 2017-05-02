# This file is used to manage SwiftShader's dependencies in the Chromium src
# repo. It is used by gclient to determine what version of each dependency to
# check out, and where.

use_relative_paths = True

vars = {
  'chromium_git': 'https://chromium.googlesource.com',
  # Current revision of subzero.
  'subzero_revision': 'fb705a6d55003b2c32772ae49e25b0babcff5acc',
}

deps = {
  'third_party/pnacl-subzero':
    Var('chromium_git') + '/native_client/pnacl-subzero@' +  Var('subzero_revision'),
}
