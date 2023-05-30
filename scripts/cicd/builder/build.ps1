# Copyright © 2022, Medelför OÜ. All rights reserved.

param (
    [Parameter(Mandatory=$true)][string]$branch,
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
    echo "Caught exit code: $LastExitCode"
    if ($LastExitCode -ne 0) {
        exit 1
    }
}

#### BUILDING PROJECT ####

# adds modules in the template's .Build.cs file, enables specified plugins
python $mbs_root/ue/template/setup.py . ue_template "UnrealisticDocs, UnrealisticDocsTests" "UnrealisticDocs, UnrealisticDocsTests"
Check-ExitCode

if ($template -eq "ue4.27.2") {
    $UE_ROOT = "$env:UE_4_27_ROOT"
    $IS_UE5 = "false"
} elseif ($template -eq "ue5.0") {
    $UE_ROOT = "$env:UE_5_0_ROOT"
    $IS_UE5 = "true"
} elseif ($template -eq "ue5.1") {
    $UE_ROOT = "$env:UE_5_1_ROOT"
    $IS_UE5 = "true"
} else {
    echo "Undefined template: ${template}"
    exit 1
}

echo "UE_ROOT: $UE_ROOT"
$wd = (Get-Item .).FullName

python $mbs_root/ue/builder/build.py "$UE_ROOT" "$wd/ue_template.uproject" "$IS_UE5"
Check-ExitCode

#### COPYING DIST FILES INTO dist_src ####

Remove-Item dist_src -Recurse -ErrorAction Ignore
mkdir -Force dist_src

$plugin_root = "Plugins/UnrealisticDocs"

# copy Config, Source, Content, .uplugin
Copy-Item -Path "$plugin_root/Config" -Destination dist_src -Recurse -Force
Check-ExitCode

Copy-Item -Path "$plugin_root/Source" -Destination dist_src -Recurse -Force
Check-ExitCode

# no content yet
#Copy-Item -Path "$plugin_root/Content" -Destination dist_src -Recurse -Force
#Check-ExitCode

Copy-Item -Path "$plugin_root/UnrealisticDocs.uplugin" -Destination dist_src -Force
Check-ExitCode

# copies additional files specified by FilterPlugin.ini
python $mbs_root/ue/filter/filter.py "$plugin_root/Config/FilterPlugin.ini" "$plugin_root" dist_src
Check-ExitCode
