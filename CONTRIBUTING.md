# Contributing to Oblivion Run

Thanks for your interest in improving the game. Bug reports, ideas,
documentation improvements, and code contributions are welcome.

## Before you start

- Search existing issues and discussions for related reports.
- For a substantial feature or a change to game behavior, open an issue first
  to agree on scope before investing in an implementation.
- Keep changes focused and avoid committing generated executables, local score
  data, editor settings, or unrelated formatting changes.

## Development setup

Follow [`getstart.md`](getstart.md) to install the Windows/MinGW-w64 toolchain
and build from the repository root. Raylib is bundled under
`third_party/raylib/`.

There is currently no automated test suite. Before submitting a code change:

1. Build with the documented command and its `-Wall -Wextra` warnings enabled.
2. Run `.\build\main.exe` from the repository root and exercise the changed
   behavior.
3. For gameplay changes, smoke-test a representative run: menu and difficulty
   selection, name entry, tutorial, movement and combat, pause/resume, damage,
   and game over as relevant.
4. Review `git diff` and confirm that no build output, high scores, or unrelated
   files are included.

For documentation-only changes, review rendered Markdown and verify that
relative links point to existing files.

## Code and asset guidelines

- Follow the naming, formatting, and C patterns already used by nearby code.
- Keep shared state and tuning consistent with `include/types.h` and
  `include/constants.h`; avoid adding global state or per-frame allocations.
- Preserve the documented update and draw ordering unless the change requires
  and verifies an intentional behavior change.
- Keep asset references relative to the repository root and wire newly loaded
  resources into the appropriate cleanup path.
- Submit only assets you created or are allowed to redistribute, and include
  any required attribution or license information.
- Update directly related documentation when behavior, controls, or setup
  instructions change.

## Pull requests

Open a pull request against the default branch. Include:

- A concise summary of the change and its motivation.
- The relevant issue link, if there is one.
- The build and manual checks performed, or a clear note about anything not
  tested.
- Screenshots or a short recording for visible UI/gameplay changes, where
  practical.

Keep the pull request focused and describe any intentional behavior changes or
known limitations. Be respectful and follow the
[Code of Conduct](CODE_OF_CONDUCT.md).
