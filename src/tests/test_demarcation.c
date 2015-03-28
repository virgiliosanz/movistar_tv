#include "config.h"

const char *doc = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <ServiceDiscovery xmlns=\"urn:dvb:ipisdns:2006\" xmlns:tva=\"urn:tva:metadata:2005\" xmlns:mpeg7=\"urn:tva:mpeg7:2005\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:dvb:ipisdns:2006 C:\\DVB_IPI\\sdns.xsd\"> <ServiceProviderDiscovery Version=\"24\"> <ServiceProvider DomainName=\"DEM_1.imagenio.es\" Version=\"237\"> <Offering> <Push Address=\"239.0.2.140\" Port=\"3937\"> </Push> </Offering> </ServiceProvider> <ServiceProvider DomainName=\"DEM_4.imagenio.es\" Version=\"236\"> <Offering> <Push Address=\"239.0.2.142\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_6.imagenio.es\" Version=\"237\"> <Offering> <Push Address=\"239.0.2.144\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_10.imagenio.es\" Version=\"239\"> <Offering> <Push Address=\"239.0.2.146\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_12.imagenio.es\" Version=\"236\"> <Offering> <Push Address=\"239.0.2.148\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_13.imagenio.es\" Version=\"239\"> <Offering> <Push Address=\"239.0.2.150\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_15.imagenio.es\" Version=\"238\"> <Offering> <Push Address=\"239.0.2.152\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_24.imagenio.es\" Version=\"237\"> <Offering> <Push Address=\"239.0.2.156\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_29.imagenio.es\" Version=\"236\"> <Offering> <Push Address=\"239.0.2.158\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_31.imagenio.es\" Version=\"239\"> <Offering> <Push Address=\"239.0.2.160\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_32.imagenio.es\" Version=\"236\"> <Offering> <Push Address=\"239.0.2.162\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_34.imagenio.es\" Version=\"237\"> <Offering> <Push Address=\"239.0.2.164\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_35.imagenio.es\" Version=\"239\"> <Offering> <Push Address=\"239.0.2.166\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_36.imagenio.es\" Version=\"238\"> <Offering> <Push Address=\"239.0.2.168\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_37.imagenio.es\" Version=\"237\"> <Offering> <Push Address=\"239.0.2.170\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_38.imagenio.es\" Version=\"236\"> <Offering> <Push Address=\"239.0.2.172\" Port=\"3937\"> </Push> </Offering></ServiceProvider><ServiceProvider DomainName=\"DEM_19.imagenio.es\" Version=\"237\"> <Offering> <Push Address=\"239.0.2.154\" Port=\"3937\"> </Push> </Offering></ServiceProvider></ServiceProviderDiscovery> </ServiceDiscovery>";

char * test_parse() {
	mcast_s *mcast;

	mcast = demarcation_get_mcast_group_from_xml(doc, 13);
	mu_assert(mcast != NULL, "Error reading demarcation");
	mu_assert(strcmp(mcast->ip, "239.0.2.150") == 0 , "Error reading demarcation ip: %s", mcast->ip);
	mu_assert(mcast->port == 3937 , "Error reading demarcation port: %d", mcast->port);
	trace("%s (%d)", mcast->ip, mcast->port);
	mcast_free(mcast);

	mcast = demarcation_get_mcast_group_from_xml(doc, 19);
	mu_assert(mcast != NULL, "Error reading demarcation");
	mu_assert(strcmp(mcast->ip, "239.0.2.154") == 0 , "Error reading demarcation ip: %s", mcast->ip);
	mu_assert(mcast->port == 3937 , "Error reading demarcation port: %d", mcast->port);
	trace("%s (%d)", mcast->ip, mcast->port);
	mcast_free(mcast);



	return NULL;
}

char *
all_tests()
{
	mu_suite_start();
	mu_run_test(test_parse);

	return NULL;
}

RUN_TESTS(all_tests)
