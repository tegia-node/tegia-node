# Tegia SDK Docs

This directory contains SDK-facing documentation for repositories that use `tegia-node` through public headers in `include/tegia`.

Scope:
- actor configuration repositories (`*-conf`);
- usage contracts of public Tegia APIs;
- integration notes needed outside of `tegia-node` runtime internals.

Start here:
- `tegia-node-for-configs.md` - minimal context about what `tegia-node` is and what must live in configuration repositories.
- `actor-development-contract.md` - how to define actor types, actions, and `_init_type` in shared libraries.
- `message-contract.md` - message structure, callback chain, and send API.
- `ws-routing-contract.md` - WS route declaration via `ADD_WS_ACTION_ROUTE`, mapping rules, and constraints.
- `config-repo-contract.md` - required structure of `cluster.json` and `configurations/*.json`.
- `quickstart-new-actor.md` - minimal practical checklist to start a new actor in `*-conf`.

Internal `tegia-node` architecture and runtime implementation details stay in `/docs`.
