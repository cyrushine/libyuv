vars = {
  # Override root_dir in your .gclient's custom_vars to specify a custom root
  # folder name.
  "root_dir": "libyuv",
  "extra_gyp_flag": "-Dextra_gyp_flag=0",

  # Roll the Chromium Git hash to pick up newer versions of all the
  # dependencies and tools linked to in setup_links.py.
  'chromium_revision': '441009c0b2454f93adf9d30a42eaaf466b1673a8',
}

hooks = [
  {
    # Clone chromium and its deps.
    "name": "sync chromium",
    "pattern": ".",
    "action": ["python", "-u", Var("root_dir") + "/sync_chromium.py",
               "--target-revision", Var("chromium_revision")],
  },
  {
    # Create links to shared dependencies in Chromium.
    "name": "setup_links",
    "pattern": ".",
    "action": ["python", Var("root_dir") + "/setup_links.py"],
  },
  {
    # A change to a .gyp, .gypi, or to GYP itself should run the generator.
    "pattern": ".",
    "action": ["python", Var("root_dir") + "/gyp_libyuv"],
  },
]
