/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "util/logger.h"
#include "util/exception.h"
#include "audio/fife_openal.h"

#include "soundmanager.h"
#include "soundemitter.h"
#include "soundclippool.h"

namespace FIFE {
	static Logger _log(LM_AUDIO);

	SoundManager::SoundManager() : m_context(0),
				       m_device(0),
				       m_mutevol(0),
							 m_volume(1.0) {
		// add provider to the pool
		m_pool.addResourceProvider(new SoundClipProvider());
	}

	SoundManager::~SoundManager() {

		// free all soundemitters
		std::vector<SoundEmitter*>::iterator it;
		for (it = m_emittervec.begin(); it != m_emittervec.end(); ++it) {
			if ((*it) != NULL) {
				delete (*it);
			}
		}

		m_emittervec.clear();
		alcDestroyContext(m_context);

		if (m_device) {
			alcCloseDevice(m_device);
		}
	}

	void SoundManager::init() {
		m_device = alcOpenDevice(NULL);

		if (!m_device) {
			throw Exception("could not open audio device!");
		}

		m_context = alcCreateContext(m_device, NULL);
		if (alcGetError(m_device) != ALC_NO_ERROR || !m_context) {
			throw Exception("Couldn't create audio context");
		}
		alcMakeContextCurrent(m_context);
		if (alcGetError(m_device) != ALC_NO_ERROR) {
			throw Exception("Couldn't change current audio context");
		}

		// set listener position
		alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
		ALfloat vec1[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
		alListenerfv(AL_ORIENTATION, vec1);

		// set volume
		alListenerf(AL_GAIN, m_volume);
	}

	SoundEmitter* SoundManager::getEmitter(unsigned int emitterid) {
		return m_emittervec.at(emitterid);
	}

	SoundEmitter* SoundManager::createEmitter() {
		SoundEmitter* ptr = new SoundEmitter(m_emittervec.size());
		m_emittervec.push_back(ptr);
		return ptr;
	}

	void SoundManager::releaseEmitter(unsigned int emitterid) {
		SoundEmitter** ptr = &m_emittervec.at(emitterid);
		delete *ptr;
		*ptr = NULL;
	}
} //FIFE
