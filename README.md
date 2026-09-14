# cop4610-shell

A custom Unix shell built in C, supporting I/O redirection, piping, background job control, and built-in commands (`cd`, `jobs`, `exit`).

**Course:** COP 4610 — Operating Systems, Florida State University
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
| Prompt | |
| Environment Variables | |
| Tilde Expansion | |
| $PATH Search | |
| External Command Execution | |
| I/O Redirection | |
| Piping | |
| Background Processing | |
| Internal Command Execution | |
| Extra Credit | |

## Development Log

### Gabriel Valladares-Ruiz
- [Date] — [What was worked on]
- [Date] — [What was worked on]

### Olivia Anderson
- [Date] — [What was worked on]
- [Date] — [What was worked on]

### Gannon Wooley
- [Date] — [What was worked on]
- [Date] — [What was worked on]

## Group Meetings

| Date | Attendees | Summary |
|---|---|---|
| | | |
| | | |

## Extra Credit

> List any extra credit attempted and briefly describe the implementation (required for credit per the assignment rubric).

- [ ] Unlimited pipes
- [ ] Piping + I/O redirection in a single command
- [ ] Shell-ception (running the shell from within itself)

## Known Issues / Limitations

- [List any known bugs or unimplemented edge cases here]
