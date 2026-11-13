Flood Unblock
=============

Backdrop blocks logins after too many failed attempts, either for one account or from one IP address, and does the same for password reset requests. The block lifts on its own after a while, but a member who is locked out usually wants back in now.

This module lists the failed attempts that still count, shows which are blocking someone and when each block lifts, and lets an administrator clear them.

- Covers failed logins (per account and per IP), password reset requests (per account and per IP), and wrong current passwords on the account form.
- A **Blocked logins** block is added to the admin dashboard on install, listing anyone currently blocked with a link to unblock them. It can be moved or removed in the layout editor like any other block.
- Shows the location of each IP address if [Smart IP](https://backdropcms.org/project/smart_ip) is installed.

Installation
------------

Install this module using the official Backdrop CMS instructions at https://backdropcms.org/guide/modules.

Grant the "Unblock flood-controlled logins" permission to the roles that need it. It shows IP addresses, so give it to trusted roles only.

Manage the flood entries under "Administer" -> "Configuration" -> "User Accounts" -> "Flood Unblock". The limits themselves are set under "Account settings".

Security
--------

Reviewed on 13 November 2026 for the kinds of vulnerability reported in the Drupal security advisories of 7 October to 11 November 2026. Those advisories were for Drupal 10/11 and do not apply to Backdrop directly; this module was checked for the same classes of mistake.

Please report security issues privately to security@backdropcms.org, not in the issue queue.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.

Maintainers
-----------

- [Steve Moorhouse (albanycomputers)](https://github.com/albanycomputers)
- Additional maintainers welcomed.


Credits
-------
Ported to Backdrop by:

- herbdool <https://github.com/herbdool>


This module is based on the Flood Unblock module for Drupal--starting at version 7.x-1.5--originally written and maintained by:

- fabianderijk <https://www.drupal.org/u/fabianderijk>

