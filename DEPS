# This file is used to manage the SwiftShader's dependencies in the Chromium src
# repo. It is used by gclient to determine what version of each dependency to
# check out, and where.

use_relative_paths = True

vars = {
  'chromium_git': 'https://chromium.googlesource.com/',
  # Current revision of subzero.
  'subzero_revision': 'fc8f6bfae75430b00d8d6fbf78e62da4c3abed9d',
}

deps = {
  'third_party/pnacl-subzero':
    Var('chromium_git') + '/native_client/pnacl-subzero@' +  Var('subzero_revision'),
}