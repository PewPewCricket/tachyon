#pragma once
#include <limine/limine.h>

#define __limine_request __attribute__((used, section(".limine_requests")))
#define __limine_requests_start __attribute__((used, section(".limine_requests_start")))
#define __limine_requests_end __attribute__((used, section(".limine_requests_end")))