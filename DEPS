use_relative_paths = True

vars = {
  'github': 'https://github.com',

  'effcee_revision': 'c7b4db79f340f7a9981e8a484f6d5785e24242d1',

  # Pin to the last version of googletest that supports C++11.
  # Anything later requires C++14
  'googletest_revision': 'v1.12.0',

  # Use protobufs before they gained the dependency on abseil
  'protobuf_revision': 'v3.13.0.1',

  're2_revision': '8afcf7fcc481692197e33612446d69e8f5777c54',
  'spirv_headers_revision': 'aa331ab0ffcb3a67021caa1a0c1c9017712f2f31',
}

deps = {
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
}

