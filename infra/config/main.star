#!/usr/bin/env lucicfg

luci.project(
    name = "swiftshader",
)

luci.cq_group(
    name = 'SwiftShader-CQ',
    watch = cq.refset('https://swiftshader.googlesource.com/SwiftShader'),
    acls = [
        acl.entry(
            acl.CQ_COMMITTER,
            groups = 'project-swiftshader-committers',
        ),
        acl.entry(
            acl.CQ_DRY_RUNNER,
            groups = 'project-swiftshader-tryjob-access',
        ),
    ],
    verifiers = [
        luci.cq_tryjob_verifier(
            builder = 'chromium:try/linux-swangle-try-tot-swiftshader-x64',
        ),
        luci.cq_tryjob_verifier(
            builder = 'chromium:try/win-swangle-try-tot-swiftshader-x86',
        ),
    ],
)

luci.cq(
    status_host = 'chromium-cq-status.appspot.com',
)
