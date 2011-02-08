/* $Id$ */
/*
   Copyright (C) 2006 - 2011 by Karol Nowak <grzywacz@sul.uni.lodz.pl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/
#ifndef SOUNDSOURCE_HPP_INCLUDED
#define SOUNDSOURCE_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "generic_event.hpp"
#include "map_location.hpp"
#include "savegame_config.hpp"

class config;
class display;

namespace soundsource {

class sourcespec;
class manager;

/*
 * Sound source is an object on a map (a location) which has one or more
 * sounds effects associated with it, which are played randomly and with
 * appropriate delays, when sound emiting object is visible on screen.
 */
class positional_source {
	unsigned int last_played_;
	unsigned int min_delay_;
	unsigned int chance_;
	unsigned int loops_;
	const unsigned int id_;
	unsigned int range_;
	unsigned int faderange_;
	bool check_fogged_;
	bool check_shrouded_;
	std::string files_;
	std::vector<map_location> locations_;

	// Last assigned id; this can, of course, overflow, but I'd
	// never expect to see 4 billions sound sources being created...
	static unsigned int last_id;

public:
	// min_delay is a minimum time in seconds, which must pass before
	// this sound source can be played again if it remains visible
	//
	// chance is a chance ;-) (in %) that the sound source will emit
	// sound every second after the delay has passed or once the source
	// becomes visible
	positional_source(const sourcespec &spec);
	~positional_source();

	bool is_global();

	void update(unsigned int time, const display &disp);
	void update_positions(unsigned int time, const display &disp);

	int calculate_volume(const map_location &loc, const display &disp);

	/**
	 * Serializes attributes as WML config.
	 * @param cfg A reference to a [sound_source] tag object.
	 */
	void write_config(config& cfg) const;
};

class manager : public events::observer, public savegame::savegame_config {

	typedef std::map<std::string, positional_source *> positional_source_map;
	typedef positional_source_map::iterator            positional_source_iterator;
	typedef positional_source_map::const_iterator      positional_source_const_iterator;

	positional_source_map sources_;
	const display &disp_;

public:
	manager(const display &disp);
	~manager();

	// event interface
	void handle_generic_event(const std::string &event_name);

	// add or replace a soundsource
	void add(const sourcespec &source);
	void remove(const std::string &id);
	void update();

	// checks which sound sources are visible
	void update_positions();

	/**
	 * Serializes information into cfg as new childs of key
	 * "sound_source", appendend to existing content.
	 */
	void write_sourcespecs(config& cfg) const;

	config to_config() const;
};

/**
 * Sound source info class.
 * Encapsulates sound source parameters, so that they're easier to pass
 * around/extend/read.
 */
class sourcespec
{
	const std::string id_;
	const std::string files_;

	int min_delay_;
	int chance_;

	int loops_;
	int range_;
	int faderange_;
	bool check_fogged_;
	bool check_shrouded_;

	std::vector<map_location> locations_;

public:
	/** Parameter-list constructor. */
	sourcespec(const std::string& id, const std::string& files, int min_delay, int chance) :
		id_(id),
		files_(files),
		min_delay_(min_delay),
		chance_(chance),
		loops_(0),
		range_(3),
		faderange_(14),
		check_fogged_(false),
		check_shrouded_(false),
		locations_()
	{}

	/** WML constructor. */
	sourcespec(const config& cfg);

	/**
	 * Serializes information into cfg as a new (appended)
	 * child of key "sound_source".
	 */
	void write(config& cfg) const;

	int loops() const { return loops_; }

	void set_loops(int value) {
		loops_ = value;
	}

	bool check_fogged() const { return check_fogged_; }
	bool check_shrouded() const { return check_shrouded_; }

	void set_check_fogged(bool value) {
		check_fogged_ = value;
	}

	void set_check_shrouded(bool value) {
		check_shrouded_ = value;
	}

	const std::vector<map_location>& get_locations() const {
		return locations_;
	}

	int full_range() const { return range_; }

	void set_full_range(int value) {
		range_ = value;
	}

	int fade_range() const { return faderange_; }

	void set_fade_range(int value) {
		faderange_ = value;
	}

	int minimum_delay() const { return min_delay_; }

	void set_minimum_delay(int value) {
		min_delay_ = value;
	}

	int chance() const { return chance_; }

	void set_chance(int value) {
		chance_ = value;
	}

	const std::string& id() const { return id_; }

	const std::string& files() const { return files_; }
};

} // namespace soundsource

#endif
