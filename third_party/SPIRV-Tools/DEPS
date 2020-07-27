use_relative_paths = True

vars = {
  'github': 'https://github.com',

  'effcee_revision': '5af957bbfc7da4e9f7aa8cac11379fa36dd79b84',
  'googletest_revision': '011959aafddcd30611003de96cfd8d7a7685c700',
  're2_revision': 'aecba11114cf1fac5497aeb844b6966106de3eb6',
  'spirv_headers_revision': 'ac638f1815425403e946d0ab78bac71d2bdbf3be',
}

deps = {
  'external/effcee':
      Var('github') + '/google/effcee.git@' + Var('effcee_revision'),

  'external/googletest':
      Var('github') + '/google/googletest.git@' + Var('googletest_revision'),

  'external/re2':
      Var('github') + '/google/re2.git@' + Var('re2_revision'),

  'external/spirv-headers':
      Var('github') +  '/KhronosGroup/SPIRV-Headers.git@' +
          Var('spirv_headers_revision'),
}

