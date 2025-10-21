AGENTS

Scope
- Applies to `uppsrc/Painter`.

Purpose
- High-quality 2D vector renderer with path/rasterization comparable to PDF/SVG; integrates with Draw.

Key Areas
- `Painter` core (paths, stroker/dasher, transforms, rasterizer, alpha, gradients).
- SVG: parsing and rendering helpers.

Extension Points
- Add new path effects or fills in dedicated modules; keep the rendering pipeline modular.

.upp Notes
- Ensure `AGENTS.md` is the first file in `Painter.upp`.

