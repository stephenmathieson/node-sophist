{
  'targets': [
    {
      'target_name': 'sophia-cc',
      'product_prefix': 'lib',
      'type': 'static_library',
      'include_dirs': [
        'sophia.cc/deps/list',
        'sophia/db',
        'sophia.cc',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'sophia.cc/deps/list',
          'sophia/db',
          'sophia.cc',
        ]
      },
      'dependencies': [
        'sophia/sophia.gyp:sophia'
      ],
      'sources': [
        'sophia.cc/sophia.cc',
        'sophia.cc/deps/list/list.c',
        'sophia.cc/deps/list/list_iterator.c',
        'sophia.cc/deps/list/list_node.c',
      ]
    }
  ]
}
