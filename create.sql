-- MySQL Script generated by MySQL Workbench
-- Mon Oct 19 15:54:00 2015
-- Model: New Model    Version: 1.0
-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema keijiban
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `keijiban` ;

-- -----------------------------------------------------
-- Schema keijiban
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `keijiban` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci ;
-- -----------------------------------------------------
-- Schema new_schema1
-- -----------------------------------------------------
USE `keijiban` ;

-- -----------------------------------------------------
-- Table `keijiban`.`users`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `keijiban`.`users` (
  `id` INT NOT NULL AUTO_INCREMENT COMMENT '',
  `name` VARCHAR(50) NULL COMMENT '',
  `passwd` VARCHAR(50) NULL COMMENT '',
  `email` VARCHAR(200) NULL COMMENT '',
  PRIMARY KEY (`id`)  COMMENT '')
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `keijiban`.`articles`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `keijiban`.`articles` (
  `id` INT NOT NULL AUTO_INCREMENT COMMENT '',
  `title` VARCHAR(50) NULL COMMENT '',
  `body` TEXT NULL COMMENT '',
  `user_id` INT NULL COMMENT '',
  `created_date` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '',
  PRIMARY KEY (`id`)  COMMENT '',
  CONSTRAINT `user_id`
    FOREIGN KEY (`id`)
    REFERENCES `keijiban`.`users` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

USE `keijiban` ;

-- -----------------------------------------------------
-- Placeholder table for view `keijiban`.`view1`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `keijiban`.`view1` (`id` INT);

-- -----------------------------------------------------
--  routine1
-- -----------------------------------------------------

DELIMITER $$
USE `keijiban`$$
$$

DELIMITER ;

-- -----------------------------------------------------
-- View `keijiban`.`view1`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `keijiban`.`view1`;
USE `keijiban`;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

