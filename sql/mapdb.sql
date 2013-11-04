-- phpMyAdmin SQL Dump
-- version 3.5.8.1deb1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Nov 04, 2013 at 02:21 PM
-- Server version: 5.5.34-0ubuntu0.13.04.1-log
-- PHP Version: 5.4.9-4ubuntu2.3

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `mapdb`
--

-- --------------------------------------------------------

--
-- Table structure for table `cmd`
--

CREATE TABLE IF NOT EXISTS `cmd` (
  `uid` int(11) NOT NULL AUTO_INCREMENT,
  `command` int(11) NOT NULL,
  `parameter` int(11) NOT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `completed` int(11) NOT NULL,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=442 ;

-- --------------------------------------------------------

--
-- Table structure for table `floorplan`
--

CREATE TABLE IF NOT EXISTS `floorplan` (
  `uid` int(11) NOT NULL AUTO_INCREMENT,
  `x_coord` int(11) NOT NULL,
  `y_coord` int(11) NOT NULL,
  `loc_type` int(11) NOT NULL,
  `stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=717900 ;

-- --------------------------------------------------------

--
-- Table structure for table `gps`
--

CREATE TABLE IF NOT EXISTS `gps` (
  `uid` int(11) NOT NULL AUTO_INCREMENT,
  `sentence` varchar(8) NOT NULL,
  `status` int(11) NOT NULL,
  `latitude` float NOT NULL,
  `longitude` float NOT NULL,
  `time_utc` varchar(32) NOT NULL,
  `altitude` float NOT NULL,
  `eps` float NOT NULL,
  `epx` float NOT NULL,
  `epv` float NOT NULL,
  `ept` float NOT NULL,
  `speed` float NOT NULL,
  `climb` float NOT NULL,
  `track` float NOT NULL,
  `mode` int(11) NOT NULL,
  `fix` int(11) NOT NULL,
  `sats` varchar(255) NOT NULL,
  `stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=9414 ;

-- --------------------------------------------------------

--
-- Table structure for table `location`
--

CREATE TABLE IF NOT EXISTS `location` (
  `uid` int(11) NOT NULL AUTO_INCREMENT,
  `x_coord` int(11) NOT NULL,
  `y_coord` int(11) NOT NULL,
  `loc_type` int(11) NOT NULL,
  `stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`uid`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;

-- --------------------------------------------------------

--
-- Table structure for table `log`
--

CREATE TABLE IF NOT EXISTS `log` (
  `uid` int(11) NOT NULL AUTO_INCREMENT,
  `cmd_uid` int(11) NOT NULL,
  `command` int(11) NOT NULL,
  `parameter` int(11) NOT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `completed` int(11) NOT NULL,
  PRIMARY KEY (`uid`),
  UNIQUE KEY `cmd_uid` (`cmd_uid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `scanning`
--

CREATE TABLE IF NOT EXISTS `scanning` (
  `uid` int(11) NOT NULL AUTO_INCREMENT,
  `gps_uid` int(11) NOT NULL,
  `cmd_uid` int(11) NOT NULL,
  `ardtime` varchar(20) DEFAULT NULL,
  `heading` float NOT NULL,
  `offset` float NOT NULL,
  `radius` float NOT NULL,
  `cart_x` float NOT NULL,
  `cart_y` float NOT NULL,
  `stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`uid`),
  KEY `gps_uid` (`gps_uid`,`cmd_uid`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=12637 ;

-- --------------------------------------------------------

--
-- Table structure for table `telemetry`
--

CREATE TABLE IF NOT EXISTS `telemetry` (
  `uid` int(11) NOT NULL AUTO_INCREMENT,
  `gps_uid` int(11) NOT NULL,
  `cmd_uid` int(11) NOT NULL,
  `ardtime` varchar(20) NOT NULL,
  `motion` int(11) NOT NULL,
  `accel_x` float NOT NULL,
  `accel_y` float NOT NULL,
  `accel_z` float NOT NULL,
  `heading` float NOT NULL,
  `offset` int(11) NOT NULL,
  `directahead` float NOT NULL,
  `directbehind` float NOT NULL,
  `directright` float NOT NULL,
  `directleft` float NOT NULL,
  `ticksright` int(11) NOT NULL,
  `ticksleft` int(11) NOT NULL,
  `voltage` int(11) NOT NULL,
  `stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`uid`),
  KEY `time` (`ardtime`),
  KEY `gps_uid` (`gps_uid`),
  KEY `cmd_uid` (`cmd_uid`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=111660 ;

-- --------------------------------------------------------

--
-- Table structure for table `webcam`
--

CREATE TABLE IF NOT EXISTS `webcam` (
  `uid` int(11) NOT NULL AUTO_INCREMENT,
  `gps_uid` int(11) NOT NULL,
  `cmd_uid` int(11) NOT NULL,
  `camimage` mediumblob NOT NULL,
  `thumb` mediumblob NOT NULL,
  `stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`uid`),
  KEY `gps_uid` (`gps_uid`,`cmd_uid`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=610 ;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
