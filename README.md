# cop4610-shell

A custom Unix shell built in C, supporting I/O redirection, piping, background job control, and built-in commands (`cd`, `jobs`, `exit`).

**Course:** COP 4610 — Operating Systems
**Project:** Project 1 — Build a Shell with Fancy Features
**Group:** 12

## Team Members

1. Gabriel Valladares-Ruiz
2. Olivia Anderson
3. Gannon Wooley

## File Listing

```
cop4610-shell/
├── src/                # All .c source files
│   ├── main.c
│   ├── parser.c
│   ├── executor.c
│   ├── redirection.c
│   ├── pipes.c
│   ├── background.c
│   └── builtins.c
├── include/             # All .h header files
│   ├── parser.h
│   ├── executor.h
│   ├── redirection.h
│   ├── pipes.h
│   ├── background.h
│   └── builtins.h
├── bin/                 # Compiled executable (produced by `make`)
│   └── shell
├── Makefile
├── README.md
└── division_of_labor.md
```

> Update this listing as files are added or renamed — it should always match what's actually in the repo.

## How to Compile and Run

Compile:

```bash
make
```

This produces the `shell` executable in the `bin/` directory.

Run:

```bash
./bin/shell
```

Clean build artifacts:

```bash
make clean
```

**Tested environment:** linprog (linprog2.cs.fsu.edu)

## Project Structure

- `src/` — All source files, split by functionality (parsing, execution, redirection, piping, background jobs, built-ins).
- `include/` — All header files.
- `bin/` — Build output only (`shell` executable). Not checked into version control.
- `Makefile` — Builds `shell` into `bin/`.

## Division of Labor (Before)

See [`division_of_labor.md`](./division_of_labor.md) for the original task assignments submitted before development began.

## Division of Labor (After)

> Fill this in at the end of the project with what was *actually* completed by each member — this can differ from the original plan.

| Part | Completed By |
|---|---|
| Prompt | olivia|
| Environment Variables | olivia |
| Tilde Expansion | gannon |
| $PATH Search | gabe |
| External Command Execution | olivia |
| I/O Redirection | |
| Piping | |
| Background Processing | |
| Internal Command Execution | |
| Extra Credit | |

## Development Log

### Gabriel Valladares-Ruiz
- [9/21/26] — [debugged linprog ssh on computer, implemented part 4 path search]
- [Date] — [What was worked on]

### Olivia Anderson
- [9/14/26] — [made github link, created division of labor, and started README]
- [9/20/26] — [added skeleton: prompt loop, tokenizer, stubbed modules]
- [9/21/26] — [implemented environment variable expansion, wire into main loop, added .gitignore for bin/obj, and external command execution]
- [9/22/26] — [implemented builtin_cd]

### Gannon Wooley
- [9/21/26] — [implemented part 3 tilde expression, remove build artifacts from version control]
- [Date] — [What was worked on]

## Group Meetings

| Date | Attendees | Summary |
|---|---|---|
| 9/21/26 | everyone | ensured project was up and running on everyone's computer. completed parts 1-5 |
| | | |

## Extra Credit

> List any extra credit attempted and briefly describe the implementation (required for credit per the assignment rubric).

- [ ] Unlimited pipes
- [ ] Piping + I/O redirection in a single command
- [ ] Shell-ception (running the shell from within itself)

## Known Issues / Limitations

- [List any known bugs or unimplemented edge cases here]
