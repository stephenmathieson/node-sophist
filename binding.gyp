{
  "targets": [
    {
      "target_name": "sophist",
      "dependencies": [
        "<(module_root_dir)/deps/sophia.cc.gyp:sophia-cc",
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
      ],
      "sources": [
        "src/binding.cc",
        "src/sophist.cc",
        "src/iterator.cc",
        "src/transaction.cc",
      ]
    }
  ]
}
