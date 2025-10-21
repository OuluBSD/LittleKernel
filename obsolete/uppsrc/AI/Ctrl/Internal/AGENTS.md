Purpose: Internal subpages used by other biography/platform controls (not standalone). Provide messaging/thread views and prompt cluster exploration.

Internal Pages
--------------
- `BiographyPlatformCtrl::Platforms::Messaging` (Messaging.cpp):
  - Three-pane view: entries (sub-forums), threads, and comments with rich entry form. Manages selection and value changes, includes list menus.
- `BiographyPlatformCtrl::Clusters` (Clusters.cpp):
  - Shows prompt clusters by image type; lists prompts and displays a synthesized final prompt with 4-image preview slots.

Notes
-----
- These pages are instantiated and hosted by `BiographyPlatformCtrl` and rely on its dataset context (profile/platform analysis). They are not intended to be registered as independent component ctrls.

