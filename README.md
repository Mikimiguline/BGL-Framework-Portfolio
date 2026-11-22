# Modular Data-Driven Gameplay & AI Framework — Unreal Engine 5.5 / C++

> 📄 **Start here: [README.pdf](README.pdf)** — the full guide, with the architecture diagram, the design rules and a reading guide to the code.
> More detail in [Documentation](Documentation/).
>
> 🎬 **Video:** [https://vimeo.com/1249170399](https://vimeo.com/1249170399)

---

## Summary

A gameplay and AI framework built from scratch in UE5 C++. Characters, Pawns and Actors are assembled from **Data Asset extensions** instead of C++ subclasses. Combat AI is coordinated by a **central token director**, and a **data-driven LOD layer** keeps **300 AI agents in active combat under 5 ms of game thread**.

| | |
|---|---|
| **Role focus** | Gameplay / AI Architecture, tools for designers |
| **Engine** | Unreal Engine 5.5, C++ |
| **Full project** | 6 plugins, ~300 source files |
| **This repository** | The headers of the 62 curated files and four complete implementations; the rest of the code is private, available on request |
| **Benchmark** | 300 AI agents in combat, **< 5 ms game thread**; the AI itself costs about **1 µs per agent** |

**Architecture.** Three layers of plugins: a genre-agnostic **Core**, a **Universal AI** layer for decision making, and **Game Features** (AI combat, perception, health, player input) injected at runtime by Gameplay Tag. A layer can use the layers above it, never the ones below.

**Key rules.** As few components per actor as possible · everything addressed by Gameplay Tags · decoupled communication through interfaces · work done once per pass for the whole population · polymorphic data as `FInstancedStruct`, not UObjects.

## Documentation

| PDF | What it covers |
|---|---|
| [README.pdf](README.pdf) | Overview, architecture diagram, rules, reading guide to the code |
| [Architecture](Documentation/ARCHITECTURE.pdf) | Layers, runtime composition, data flow per frame |
| [Code highlights](Documentation/CODE_HIGHLIGHTS.pdf) | Short excerpts with the reasoning behind each design decision |
| [Designer workflow](Documentation/DESIGNER_WORKFLOW.pdf) | What a designer can do without code, and why it is built that way |
| [Performance](Documentation/PERFORMANCE.pdf) | Benchmark, cost per AI, where the savings come from |

---

© Miguel Angel Monreal. All rights reserved. This code is published for evaluation of my work only. No licence is granted to use, copy, modify or redistribute it, in whole or in part.
