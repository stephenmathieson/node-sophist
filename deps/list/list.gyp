{
  "targets": [
    {
      "target_name": "list",
      "product_prefix": "lib",
      "type": "static_library",
      "direct_dependent_settings": {
        "include_dirs": ["."]
      },
      "sources": [
        "list.h",
        "list.c",
        "list_iterator.c",
        "list_node.c",
      ]
    }
  ]
}
