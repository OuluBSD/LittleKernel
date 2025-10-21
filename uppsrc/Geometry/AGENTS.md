AGENTS

Scope
- Applies to `uppsrc/Geometry`.

Purpose
- Geometry math and 2D/3D primitives, models, mesh processing, camera/frustum utilities, and optional importers.

Key Areas
- Math & utils: `Matrix*`, `Util.*`, common types, BBox.
- Geometry: `Geom2D/3D`, `Quadtree`, `Octree`, trackers, point clouds.
- Rendering helpers: `Draw.*`, `ModelPainter`, `ModelDraw`.
- Assets: `Mesh`, `Model`, `Material`, `ModelBuilder`.
- View/camera: `Camera`, `Frustum`, VR helpers.

Dependencies
- `plugin/mikktspace` (tangents), optional `plugin/tiny_gltf`, and Assimp (via `plugin/assimp` or system libs).

.upp Notes
- Ensure `AGENTS.md` is first in `Geometry.upp`.

