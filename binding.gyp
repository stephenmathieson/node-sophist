{
  'targets': [
    {
      'target_name': 'sophist',
      'include_dirs': [
        "<!(node -e \"require('nan')\")",
      ],
      'sources': [
        'src/sophist.h',
        'src/binding.cc',
        'src/database.h',
        'src/database.cc',
        'src/database_workers.h',
        'src/database_workers.cc',
        'src/iterator.h',
        'src/iterator.cc',
        'src/iterator_workers.h',
        'src/iterator_workers.cc',
      ],
      'dependencies': [
        'deps/sophia.cc.gyp:sophia-cc',
      ],
    },
  ],
}
