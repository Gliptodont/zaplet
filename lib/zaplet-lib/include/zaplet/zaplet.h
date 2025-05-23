/*
 * Code written by Максим Пискарёв "gliptodont"
 */

#ifndef ZAPLET_H
#define ZAPLET_H

#include "zaplet/version.h"

// config
#include "zaplet/ini/INIreader.h"

// logging
#include "zaplet/logging/log_config.h"
#include "zaplet/logging/logger.h"
#include "zaplet/logging/utils.h"

// http
#include "zaplet/http/client.h"
#include "zaplet/http/request.h"
#include "zaplet/http/response.h"
#include "zaplet/http/utils.h"

// output
#include "zaplet/output/formatter_factory.h"
#include "zaplet/output/formatter.h"

// scenario
#include "zaplet/scenario/scenario.h"
#include "zaplet/scenario/yaml_parser.h"
#include "zaplet/scenario/player.h"

#endif // ZAPLET_H
