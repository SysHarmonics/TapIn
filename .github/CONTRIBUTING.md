# Contributing to TapIn

First off, thanks for taking the time to contribute — we appreciate your interest in improving TapIn!

This project is still early in its development, so we welcome contributions that align with its goals and maintain its quality and security.

---

## 🚀 Getting Started

To contribute, first:

1. Fork the repository.
2. Clone your fork:
   ```bash
   git clone https://github.com/YOUR_USERNAME/TapIn.git
   cd TapIn
3. Build the project
   ```bash
   make
   ```
   To run tests:
   ```bash
   make test
   ```
3. Add the Upstream Remote

    This keeps your fork up-to-date with the original project:
    
    ```bash
    git remote add upstream https://github.com/ORIGINAL_OWNER/PROJECT_NAME.git
    git remote -v   # check remotes


    origin → your fork
    upstream → the original project

4. Create a New Branch

    Always create a new branch for your feature or fix:

    ```bash
    git checkout -b feature/short-descriptive-name
    ```

    This keeps main (or master) clean, and makes your PR easier to review.

5. Make Changes and Commit

    Edit code, add tests, or update documentation. Then:

    ```bash
    git add .
    git commit -m "Brief: explain what you changed"

6. Keep Branch in Sync (optional but good practice)

    Fetch latest changes from upstream and rebase:

    ```bash
    git fetch upstream
    git rebase upstream/main
    ```

    This ensures your branch is based on the latest project state.

7. Push Your Branch

    Push your branch to your fork:

    ```bash
    git push origin feature/short-descriptive-name
    ```
8. Open a Pull Request (PR)

    Go to your fork on GitHub.
    You’ll see a banner suggesting to open a PR.
    Choose base repository = original project and base branch = main 
    Compare with your feature branch.
    Add a clear title and description.

9. PR Review and Changes

    Maintainers may request changes → update your local branch, commit again, and push. The PR updates automatically.

    Keep discussion constructive and aligned with the [code of conduct](/.github/CODE_OF_CONDUCT.md).

10. PR Merge

    Once approved, a maintainer merges your branch into the upstream project.

    You can then delete your branch both locally and on GitHub.

### ✅ What Makes a Good PR - Acceptable Contributions

New features or improvements to encryption, socket handling, or UX

Portability enhancements (e.g., Windows support, cross-compilation)

Bug fixes or vulnerability patches

Additional unit tests for edge cases or new functionality

Documentation updates (README, man page, usage examples)

#### ⚠️ Contributions We May Not Accept

Committed test logs or output files

Superficial or stylistic-only edits

Large-scale refactoring without prior discussion in an issue

##### Pull Request Guidelines

Use descriptive commit messages (e.g., Fix: memory leak in handshake)

Open an issue first for large or structural changes

Keep PRs focused and minimal

Run make test before pushing

Avoid widespread style/formatting changes unless scoped

###### 🙋‍♂️ Questions or Ideas?

Have an idea but not sure if it fits? Open an issue or start a discussion. It helps others stay aligned and avoids duplicated effort.

Thanks again for helping make TapIn better!

— @SysHarmonics
