#include "core/minunit.h"
#include "bstrlib/bstrlib.h"
#include <string.h>
#include <stdlib.h>
#include <yajl/yajl_parse.h>

const char *platform_json = "{\"resultCode\":0, \"resultText\":\"Operation successful\", \"resultData\":{\"dvbConfig\":{\"dvbServiceProvider\":\"imagenio.es\",\"dvbEntryPoint\":\"239.0.2.129:3937\"},\"cdvrErrorsRetry\":\"404|453|500\",\"gfxModesString\":\"0602##\",\"tvReminderWakeUpTime\":60,\"defaultChannel\":0,\"cdvrNumberRetries\":1,\"migrationEnabledStb\":\"101\",\"personalRecordingCheckingPeriod\":120,\"migrationMode\":0,\"minimumBitrateSTB\":1000,\"vksUrl\":\"vks.svc.imagenio.telefonica.net\",\"cdvrDomainPrefix\":\"cdvr\",\"clientVersion\":\"0602\",\"viewRightActivationWithRecServices\":0,\"pvrExtraTimePost\":0,\"updateTime\":86400,\"serviceProfiles\":[{\"uplinkBW\":50,\"code\":\"SERV1\",\"downlinkBW\":500},{\"uplinkBW\":0,\"code\":\"SERV2\",\"downlinkBW\":3250},{\"uplinkBW\":0,\"code\":\"SERV3\",\"downlinkBW\":10700},{\"uplinkBW\":0,\"code\":\"SERV4\",\"downlinkBW\":270},{\"uplinkBW\":10,\"code\":\"SERV5\",\"downlinkBW\":3400},{\"uplinkBW\":10,\"code\":\"SERV6\",\"downlinkBW\":11500},{\"uplinkBW\":212,\"code\":\"SERV7\",\"downlinkBW\":212}],\"contextSensitivePromotions\":1,\"stbReSignOnThreshold\":7,\"amsConfig\":{\"amsUrl\":\"http://172.26.23.6:8889/XML_info_treat.php\",\"amsFlush\":1},\"appVersion\":\"6.0\",\"ppvBySubscription\":0,\"backwardOPCH\":\"239.0.2.10:22222\",\"epgGridPeriod\":1,\"pvrExtraTimePre\":0,\"gfxModes\":{},\"retriesNumber\":0,\"retriesPeriod\":1,\"defaultChannelPriority\":1}, \"hashCode\":\"fd03483119ecebb5e65284660a32dfc2\"}";

const char *client_json = "{\"resultCode\":0, \"resultText\":\"Operation successful\", \"resultData\":{\"uplinkBW\":1024,\"clientID\":4229164,\"purchasePIN\":\"E930AF8B6BC6EFFF86141B95AE653EB4\",\"downlinkBW\":56832,\"publicIP\":\"10.146.246.85\",\"version\":15,\"tvWholesaler\":1,\"hasRaterPromos\":0,\"gfxClientAppUrls\":{\"720x576\":{\"clientAppUrl\":\"http://172.26.22.23:2001/appclient/main.swf\"},\"1024x576\":{\"clientAppUrl\":\"http://172.26.22.23:2001/appclient/main_hd.swf\"},\"1280x720\":{\"clientAppUrl\":\"http://172.26.22.23:2001/appclient/main_hd.swf\"}},\"partition\":1,\"clientSegment\":1,\"tvPackages\":\"UTX4X|UTX6C\",\"numIPs\":1,\"clientAppUrl\":\"http://172.26.22.23:2001/appclient/main.swf\",\"hdtv\":1,\"clientVersion\":\"0602\",\"pricePlan\":1,\"services\":\"0|-2|-1|116|117|120|130|133\",\"updateTime\":81964,\"seed\":\"8d765359caa7608b87726b2fdd896840\",\"deviceTypeID\":1,\"gmtOffset\":1,\"serviceType\":4,\"adminCode\":\"28100004404130\",\"cdvrAcceptDate\":1401135024000,\"lszID\":12802,\"subscriptionsList\":\"100084708;2|100084704;2|100084704;2|100084708;2|100084704;2\",\"tvPackagesList\":\"100084708;2|100084704;2\",\"parentalPIN\":\"E930AF8B6BC6EFFF86141B95AE653EB4\",\"homeURL\":\"about:blank\",\"demarcation\":19,\"currentOPCH\":\"239.0.2.30:22222\",\"categories\":\"Catchup|Gratuito|Te lo perdiste|ppvgratis|vbdgratis\"}, \"hashCode\":\"01eb1c7d185f54c3ee90225a6a895d1b\"}";


char *test_learn_yajl()
{
    unsigned char *str;
    bstring json;
    yajl_status stat;

    yajl_handle hand = yajl_alloc(NULL, NULL, NULL);
    yajl_config(hand, yajl_dont_validate_strings, 1);

    json = bfromcstr(platform_json);
    stat = yajl_parse(hand, json->data, blength(json));
    if (yajl_status_ok != stat) {
        str = yajl_get_error(hand, 1, json->data, blength(json));
        debug("Message parsing json: %s", str);
    }
    mu_assert(yajl_status_ok == stat, "Error parsing json");


    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_learn_yajl);
    return NULL;
}

RUN_TESTS(all_tests)

