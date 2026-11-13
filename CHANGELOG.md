# Changelog

## 1.x-1.0.0 (unreleased)

First release of the Backdrop port.

### Added
- Dashboard block "Blocked logins", placed on the admin dashboard on install and removed from every layout on uninstall.
- Password reset (per account and per IP) and current-password flood events.
- "Block lifts" and "Last attempt" columns.

### Changed
- Only attempts inside their time window are counted, as core does; lapsed entries awaiting cron are no longer shown as blocked.
- Permission marked "restrict access" and renamed "Unblock flood-controlled logins" (machine name unchanged).
- Clearing uses core's `flood_clear_event()` and is logged.

### Fixed
- Configure link pointed to a path that does not exist (izmeez, #1).
- Page crashed on PHP 8 for a deleted user's entries.
- Warnings when "flood_uid_only" is set (identifier has no IP part).

### Removed
- Drush integration (Drush does not run on Backdrop).
