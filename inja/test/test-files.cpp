// Copyright (c) 2019 Pantor. All rights reserved.

#include "test.h"

const std::string test_file_directory {"../test/data/"};

TEST(inja, loading) {
  inja::Environment env;
  json data;
  data["name"] = "Jeff";

  {
    CHECK(env.load_file(test_file_directory + "simple.txt"), "Hello {{ name }}.");
  }

  {
    CHECK(env.render_file(test_file_directory + "simple.txt", data), "Hello Jeff.");
  }

  {
    CHECK(env.render_file(test_file_directory + "include.txt", data), "Answer: Hello Jeff.");
  }

  {
    std::string path(test_file_directory + "does-not-exist");

    std::string file_error_message = "[inja.exception.file_error] failed accessing file at '" + path + "'";
    CHECK_THROWS_WITH(env.load_file(path), file_error_message.c_str());
    CHECK_THROWS_WITH(env.load_json(path), file_error_message.c_str());
  }
}

TEST(inja, complete_files) {
  inja::Environment env {test_file_directory};

  for (std::string test_name : {"simple-file", "nested", "nested-line", "html"}) {
    {
      CHECK(env.render_file_with_json_file(test_name + "/template.txt", test_name + "/data.json"),
            env.load_file(test_name + "/result.txt"));
    }
  }

  for (std::string test_name : {"error-unknown"}) {
    {
      CHECK_THROWS_WITH(env.render_file_with_json_file(test_name + "/template.txt", test_name + "/data.json"),
                        "[inja.exception.parser_error] (at 2:10) expected 'in', got 'ins'");
    }
  }
}

TEST(inja, complete_files_whitespace_control) {
  inja::Environment env {test_file_directory};
  env.set_trim_blocks(true);
  env.set_lstrip_blocks(true);

  for (std::string test_name : {"nested-whitespace"}) {
    {
      CHECK(env.render_file_with_json_file(test_name + "/template.txt", test_name + "/data.json"),
            env.load_file(test_name + "/result.txt"));
    }
  }
}

TEST(inja, global_path) {
  inja::Environment env {test_file_directory, "./"};
  inja::Environment env_result {"./"};
  json data;
  data["name"] = "Jeff";

  {
    env.write("simple.txt", data, "global-path-result.txt");

    // Fails repeatedly on windows CI
    // CHECK(env_result.load_file("global-path-result.txt") == "Hello Jeff.");
  }
}

TEST(inja, include_without_local_files) {
  inja::Environment env {test_file_directory};
  env.set_search_included_templates_in_files(false);

  {
    CHECK_THROWS_WITH(env.render_file_with_json_file("html/template.txt", "html/data.json"), "[inja.exception.render_error] (at 3:14) include '../test/data/html/header.txt' not found");
  }
}
