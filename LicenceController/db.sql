/*
SQLyog Enterprise - MySQL GUI v8.1 
MySQL - 5.1.28-rc-community : Database - qqrobot
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE DATABASE /*!32312 IF NOT EXISTS*/`qqrobot` /*!40100 DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci */;

USE `qqrobot`;

/*Table structure for table `licence` */

DROP TABLE IF EXISTS `licence`;

CREATE TABLE `licence` (
  `sid` char(32) COLLATE utf8_unicode_ci NOT NULL,
  `callback` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL,
  `overtime` int(10) unsigned DEFAULT NULL,
  `owner` varchar(32) COLLATE utf8_unicode_ci DEFAULT NULL,
  UNIQUE KEY `sid` (`sid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
