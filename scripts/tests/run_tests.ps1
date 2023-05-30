# Copyright © 2022, Medelför OÜ. All rights reserved.

param (
    [Parameter(Mandatory=$true)][string]$mbs_root,
    [Parameter(Mandatory=$true)][string]$template
)

#must be called from the project's root

trap
{
    write-output $_
    exit 1
}

function Check-ExitCode {
    if ($LastExitCode -ne 0) {
        exit 1
    }
}

$wd = (Get-Item .).FullName

Remove-Item "$wd/Content" -Recurse -ErrorAction Ignore
Remove-Item "$wd/Plugins/UnrealisticDocsTests/Content" -Recurse -ErrorAction Ignore

$exec = "Automation RunTests UnrealisticDocs"

if ($template -eq "ue4.27.2") {
    $UE_ROOT = "$env:UE_4_27_ROOT"
    $EXECUTABLE = "UE4Editor-cmd.exe"

    Copy-Item "$wd/Content_4.27" "$wd/Content" -Force -Recurse
    Copy-Item "$wd/Plugins/UnrealisticDocsTests/Content_4.27" `
        "$wd/Plugins/UnrealisticDocsTests/Content" -Force -Recurse
} elseif ($template -eq "ue5.0") {
    $UE_ROOT = "$env:UE_5_0_ROOT"
    $EXECUTABLE = "UnrealEditor-Cmd.exe"

    Copy-Item "$wd/Content_5.0" "$wd/Content" -Force -Recurse
    Copy-Item "$wd/Plugins/UnrealisticDocsTests/Content_5.0" `
        "$wd/Plugins/UnrealisticDocsTests/Content" -Force -Recurse
} elseif ($template -eq "ue5.1") {
    $UE_ROOT = "$env:UE_5_1_ROOT"
    $EXECUTABLE = "UnrealEditor-Cmd.exe"

    Copy-Item "$wd/Content_5.0" "$wd/Content" -Force -Recurse
    Copy-Item "$wd/Plugins/UnrealisticDocsTests/Content_5.0" `
        "$wd/Plugins/UnrealisticDocsTests/Content" -Force -Recurse

    $exec += "; Quit"
} else {
    echo "Undefined template: ${template}"
    exit 1
}

echo "CMD: $exec"
echo "UE_ROOT: $UE_ROOT"

& $UE_ROOT/Engine/Binaries/Win64/$EXECUTABLE "$wd/ue_template.uproject" `
    -ExecCmds="$exec" `
    -unattended -nopause `
    -NullRHI `
    -testexit="Automation Test Queue Empty" `
    -log=TestsLog.txt `
    -ReportOutputPath="$wd/"

# check the report
python $mbs_root/ue/tests/check_report.py index.json
Check-ExitCode
