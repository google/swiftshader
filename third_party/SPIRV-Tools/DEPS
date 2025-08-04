use_relative_paths = True

vars = {
  'github': 'https://github.com',

  'abseil_revision': '56945519b5d17dcec0982ac6cdf4b6420f03c9c3',

  'effcee_revision': '8ce15c424e61a94ee27b5be0ec0ed036b158e6e3',

  'googletest_revision': '373af2e3df71599b87a40ce0e37164523849166b',

  # Use protobufs before they gained the dependency on abseil
  'protobuf_revision': 'v21.12',

  're2_revision': '66a656e6d9f8b78806c76201eb03038e85103e28',

  'spirv_headers_revision': '97e96f9e9defeb4bba3cfbd034dec516671dd7a3',

  'mimalloc_revision': '09a27098aa6e9286518bd9c74e6ffa7199c3f04e',
}

deps = {
  'external/abseil_cpp':
      Var('github') + '/abseil/abseil-cpp.git@' + Var('abseil_revision'),

  'external/effcee':
      Var('github') + '/google/effcee.git@' + Var('effcee_revision'),

  'external/googletest':
      Var('github') + '/google/googletest.git@' + Var('googletest_revision'),

  'external/protobuf':
      Var('github') + '/protocolbuffers/protobuf.git@' + Var('protobuf_revision'),

  'external/re2':
      Var('github') + '/google/re2.git@' + Var('re2_revision'),

  'external/spirv-headers':
      Var('github') +  '/KhronosGroup/SPIRV-Headers.git@' +
          Var('spirv_headers_revision'),

  'external/mimalloc':
      Var('github') + '/microsoft/mimalloc.git@' + Var('mimalloc_revision'),
}

