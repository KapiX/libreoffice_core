# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_ExternalProject_ExternalProject,libabw))

$(eval $(call gb_ExternalProject_use_autoconf,libabw,build))

$(eval $(call gb_ExternalProject_register_targets,libabw,\
	build \
))

$(eval $(call gb_ExternalProject_use_externals,libabw,\
	boost_headers \
	libxml2 \
	revenge \
	zlib \
))

$(call gb_ExternalProject_get_state_target,libabw,build) :
	$(call gb_ExternalProject_run,build,\
		export PKG_CONFIG="" \
		&& MAKE=$(MAKE) ./configure \
			--with-pic \
			--enable-static \
			--disable-shared \
			--without-docs \
			--disable-tools \
			--disable-debug \
			--disable-werror \
			$(if $(verbose),--disable-silent-rules,--enable-silent-rules) \
			CXXFLAGS="$(CXXFLAGS) $(BOOST_CPPFLAGS) \
				-DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED" \
			$(if $(CROSS_COMPILING),--build=$(BUILD_PLATFORM) --host=$(HOST_PLATFORM)) \
		&& $(MAKE) \
	)

# vim: set noet sw=4 ts=4:
