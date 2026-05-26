# GPU Pathtracer

A GPU pathtracer written in C++ and GLSL, made as a project for the TSBK07 course at Linköping University. It started as a port of an earlier CPU pathtracer, then grew from there.

The rendering runs entirely in a fragment shader — one invocation per pixel, accumulating samples across frames until the image converges.

---

## How it works

Geometry and a BVH are built on the CPU and uploaded to the GPU as SSBOs. The path tracing shader traverses the BVH, tests intersections, and scatters rays based on material type. Results are blended into an accumulation texture each frame and displayed after gamma correction. Two textures are ping-ponged to avoid read/write conflicts.

Random numbers come from a PCG hash seeded per-pixel and per-frame, so the noise pattern shifts each frame and the accumulation smooths out over time.

### Materials

Four material types: diffuse (cosine-weighted hemisphere sampling), mirror (perfect reflection, doesn't spend a bounce), glass/transmissive (Fresnel with Schlick approximation, handles total internal reflection), and emissive. There's also a glossy type that blends diffuse and specular using a smoothness value, though it's not heavily tested.

Mirror and glass bounces don't count against the bounce limit. Transmissive bounces are separately capped at 10 to stop infinite loops inside thin geometry.

### BVH

Built with SAH using 16 spatial bins per axis. Falls back to a median split if no SAH split brings the cost below 1.0. The tree is flattened into pre-order layout before upload so the shader can traverse it iteratively without a stack.

### Lighting

Rectangular area lights sampled stochastically. A shadow ray is cast before adding any light contribution. Point lights are in the code but not fully wired into the path tracing loop yet.

---

## Scenes

Five built-in scenes selectable from the UI:

- **0** — Cornell-style box with two spheres
- **1** — Simple room with a handful of test objects
- **2/3** — Stanford Bunny at two levels of detail
- **4** — Room with the high-poly bunny

OBJ files can also be loaded at runtime through the file dialog.

---

## Building

Requires CMake and a C++17 compiler (built with MSVC on Windows). OpenGL 4.6. Everything else is in `external/`.

```bash
cmake -S . -B build
cmake --build build
```

Output: `build/bin/Raytracer.exe`

---

## Controls

The ImGui panel lets you switch between pathtracing and rasterization mode, adjust samples per frame (1–50) and max bounces (1–20), load OBJ files, and switch scenes. Changing anything resets the accumulation.

---

## Caveats

The OBJ loader expects UV indices to be present — it'll break on files that don't have them. The rasterization mode only does a basic normal visualization, nothing fancy. Camera mouse look exists in the code but is currently disabled.
