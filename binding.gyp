{
  "targets": [
    {
      "target_name": "sophist",
      "dependencies": [
        "<(module_root_dir)/deps/sophia/sophia.gyp:sophia"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [
        "src/binding.cc",
        "src/sophist.cc",
        "src/open.cc",
        "src/close.cc",
        "src/get.cc",
        "src/set.cc",
        "src/delete.cc",
        "src/count.cc",
        "src/clear.cc",
        "src/iterator.cc",
        "src/iterator_next.cc",
        "src/iterator_end.cc",
        "src/transaction.cc",
        "src/transaction_commit.cc",
        "src/transaction_rollback.cc",
      ]
    }
  ]
}
