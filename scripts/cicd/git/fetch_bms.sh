#!/bin/bash

# Copyright © 2022, Medelför OÜ. All rights reserved.

#must be called from the project's root
#$1 - branch of BMS to clone

mkdir /root/.ssh
echo "$BMS_BUILDBOT_SSH_KEY" >> /root/.ssh/id_rsa
chmod 400 /root/.ssh/id_rsa
cp ./scripts/cicd/git/known_hosts.github /root/.ssh/known_hosts

git clone -b $1 git@github.com:medelfor/build-master-set.git scripts/cicd/buildmasterset
