/*
 * Copyright (C) 2012, The CyanogenMod Project
 *                     Daniel Hillenbrand <codeworkx@cyanogenmod.com>
 *                     Marco Hillenbrand <marco.hillenbrand@googlemail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "macloader"
#define LOG_NDEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

#include <cutils/log.h>

#define MACADDR_PATH "/efs/wifi/.mac.info"
#define CID_PATH "/data/.cid.info"

enum Type {
    NONE,
    MURATA,
    SEMCOSH,
    SEMCOVE,
    SEMCO3RD,
    SEMCO,
    WISOL
};

int main() {
    FILE* file;
    FILE* cidfile;
    char* str;
    char mac_addr_half[9];
    int ret = -1;
    int amode;
    enum Type type = NONE;

    /* open mac addr file */
    file = fopen(MACADDR_PATH, "r");
    if (file == 0) {
        fprintf(stderr, "open(%s) failed\n", MACADDR_PATH);
        ALOGE("Can't open %s\n", MACADDR_PATH);
        return -1;
    }

    /* get and compare mac addr */
    str = fgets(mac_addr_half, 9, file);
    fclose(file);
    if (str == 0) {
        fprintf(stderr, "fgets() from file %s failed\n", MACADDR_PATH);
        ALOGE("Can't read from %s\n", MACADDR_PATH);
        return -1;
    }

    /* murata
       ref: http://hwaddress.com/?q=ACT */
    if (strncasecmp(mac_addr_half, "00:0e:6d", 9) == 0 ||
        strncasecmp(mac_addr_half, "00:13:e0", 9) == 0 ||
        strncasecmp(mac_addr_half, "00:21:e8", 9) == 0 ||
        strncasecmp(mac_addr_half, "00:26:e8", 9) == 0 ||
        strncasecmp(mac_addr_half, "00:37:6d", 9) == 0 ||
        strncasecmp(mac_addr_half, "00:60:57", 9) == 0 ||
        strncasecmp(mac_addr_half, "04:46:65", 9) == 0 ||
        strncasecmp(mac_addr_half, "10:5f:06", 9) == 0 ||
        strncasecmp(mac_addr_half, "10:a5:d0", 9) == 0 ||
        strncasecmp(mac_addr_half, "1c:99:4c", 9) == 0 ||
        strncasecmp(mac_addr_half, "14:7d:c5", 9) == 0 ||
        strncasecmp(mac_addr_half, "20:02:af", 9) == 0 ||
        strncasecmp(mac_addr_half, "40:f3:08", 9) == 0 ||
        strncasecmp(mac_addr_half, "44:a7:cf", 9) == 0 ||
        strncasecmp(mac_addr_half, "5c:da:d4", 9) == 0 ||
        strncasecmp(mac_addr_half, "5c:f8:a1", 9) == 0 ||
        strncasecmp(mac_addr_half, "78:4b:87", 9) == 0 ||
        strncasecmp(mac_addr_half, "60:21:c0", 9) == 0 ||
        strncasecmp(mac_addr_half, "88:30:8a", 9) == 0 ||
        strncasecmp(mac_addr_half, "f0:27:65", 9) == 0 ||
        strncasecmp(mac_addr_half, "fc:c2:de", 9) == 0) {
        type = MURATA;
    }

    /* semcosh */
    if (strncasecmp(mac_addr_half, "34:23:ba", 9) == 0 ||
        strncasecmp(mac_addr_half, "38:aa:3c", 9) == 0 ||
        strncasecmp(mac_addr_half, "50:cc:f8", 9) == 0 ||
        strncasecmp(mac_addr_half, "5c:0a:5b", 9) == 0 ||
        strncasecmp(mac_addr_half, "88:32:9b", 9) == 0 ||
        strncasecmp(mac_addr_half, "90:18:7c", 9) == 0 ||
        strncasecmp(mac_addr_half, "cc:3a:61", 9) == 0) {
        type = SEMCOSH;
    }

    /* semco3rd */
    if (strncasecmp(mac_addr_half, "f4:09:d8", 9) == 0) {
        type = SEMCO3RD;
    }

    /* semco */
    if (strncasecmp(mac_addr_half, "c0:bd:d1", 9) == 0 ||
        strncasecmp(mac_addr_half, "51:f6:6b", 9) == 0) {
        type = SEMCO;
    }

    /* wisol */
    if (strncasecmp(mac_addr_half, "48:5A:3F", 9) == 0) {
        type = WISOL;
    }

    if (type != NONE) {
        const char *type_str;
        struct passwd *pwd;
        int fd;

        /* open cid file */
        cidfile = fopen(CID_PATH, "w");
        if(cidfile == 0) {
            fprintf(stderr, "open(%s) failed\n", CID_PATH);
            ALOGE("Can't open %s\n", CID_PATH);
            return -1;
        }

        switch(type) {
            case NONE:
                return -1;
            case MURATA:
                type_str = "murata";
                break;
            case SEMCOSH:
                type_str = "semcosh";
                break;
            case SEMCOVE:
                type_str = "semcove";
                break;
            case SEMCO3RD:
                type_str = "semco3rd";
            break;
            case SEMCO:
                type_str = "semco";
                break;
            case WISOL:
                type_str = "wisol";
            break;
        }

        ALOGI("Settting wifi type to %s in %s\n", type_str, CID_PATH);

        ret = fputs(type_str, cidfile);
        if (ret != 0) {
            ALOGE("Can't write to %s\n", CID_PATH);
            return 1;
        }

        /* Change permissions of cid file */
        ALOGD("Change permissions of %s\n", CID_PATH);

        fd = fileno(cidfile);
        amode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        ret = fchmod(fd, amode);
        if (ret != 0) {
            fclose(cidfile);
            ALOGE("Can't set permissions on %s - %s\n",
                  CID_PATH, strerror(errno));
            return 1;
        }

        pwd = getpwnam("system");
        if (pwd == NULL) {
            fclose(cidfile);
            ALOGE("Failed to find 'system' user - %s\n",
                  strerror(errno));
            return 1;
        }

        ret = fchown(fd, pwd->pw_uid, pwd->pw_gid);
        fclose(cidfile);
        if (ret != 0) {
            ALOGE("Failed to change owner of %s - %s\n",
                  CID_PATH, strerror(errno));
            return 1;
        }
    } else {
        /* delete cid file if no specific type */
        ALOGD("Deleting file %s\n", CID_PATH);
        remove(CID_PATH);
    }
    return 0;
}
