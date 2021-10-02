#!/usr/bin/env lucicfg

# Enable LUCI Realms support.
lucicfg.enable_experiment("crbug.com/1085650")

luci.project(
    name = "swiftshader",
    acls = [
        acl.entry(
            acl.PROJECT_CONFIGS_READER,
            groups = "all",
        ),
    ],
)

luci.cq_group(
    name = "SwiftShader-CQ",
    watch = cq.refset(
        repo = "https://swiftshader.googlesource.com/SwiftShader",
        refs = ["refs/heads/master"],
    ),
    acls = [
        acl.entry(
            acl.CQ_COMMITTER,
            groups = "project-swiftshader-committers",
        ),
        acl.entry(
            acl.CQ_DRY_RUNNER,
            groups = "project-swiftshader-tryjob-access",
        ),
    ],
    verifiers = [
        luci.cq_tryjob_verifier(
            builder = "chromium:try/linux-swangle-try-tot-swiftshader-x64",
        ),
        luci.cq_tryjob_verifier(
            builder = "chromium:try/win-swangle-try-tot-swiftshader-x86",
        ),
    ],
)

luci.cq(
    status_host = "chromium-cq-status.appspot.com",
)
