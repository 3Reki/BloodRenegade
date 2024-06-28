#! /bin/bash

SOURCE_FILE=$1
VERSION_NAME=$2

sed -i 's/ProjectVersion=.*/ProjectVersion='$VERSION_NAME' /' ${SOURCE_FILE}
sed -i 's/DataRouterUrl=.*/DataRouterUrl="https:\/\/c_oger_rubika_edu_com.bugsplat.com\/post\/ue4\/BloodRenegade\/'$VERSION_NAME'"/' ${SOURCE_FILE}