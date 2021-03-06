<?php

/**
 * @file
 * Main help definition.
 */

function _security_review_help() {
  drupal_add_js(drupal_get_path('module', 'security_review') . '/security_review.js');
  $output = '';
  $output .= '<p>';
  $output .= t('You should take the security of your site very seriously.
    Fortunately, Drupal is fairly secure by default.
    The Security Review module automates many of the easy-to-make mistakes that render your site insecure, however it does not automatically make your site impenetrable.
    You should give care to what modules you install and how you configure your site and server.
    Be mindful of who visits your site and what features you expose for their use.');
  $output .= '</p>';
  $output .= '<p>';
  $output .= t('You can read more about securing your site in the <a href="!do">drupal.org handbooks</a> and on <a href="!cd">CrackingDrupal.com</a>.
    There are also additional modules you can install to secure or protect your site. Be aware though that the more modules you have running on your site the greater (usually) attack area you expose.',
    array('!do' => 'http://drupal.org/security/secure-configuration', '!cd' => 'http://crackingdrupal.com'));
  $output .= '</p>';
  $output .= '<p>' . l(t('Drupal.org Handbook: Introduction to security-related contrib modules'), 'http://drupal.org/node/382752') . '</p>';
  /*$output .= '<h3>' . t('Contrib modules for additional security and strength') . '</h3>';
   $output .= '<p>' . t('There are a wealth of modules on drupal.org.') . '</p>';
   $items[] = _security_review_help_ssl();
   $items[] = _security_review_help_spam();
   $output .= theme('item_list', $items);*/
  return $output;
}

function _security_review_help_ssl() {
  $description = t('The transfer of data between the visitor on your site and you Drupal installation can be secured through encryption.');
  $element = array(
    'problem' => t("Secure and private communication"),
    'type' => 'ssl',
    'description' => $description,
    'options' => array(
      array(
        'name' => 'Secure Pages',
        'href' => 'http://drupal.org/project/securepages',
      ),
    ),
  );
  return theme('security_review_help_options', array('element' => $element));
}

function _security_review_help_spam() {
  $element = array(
    'problem' => t("Spammers and spam content"),
    'type' => 'spam',
    'description' => t('Spammers use automated tools and systems to bombarge your site with unsollicited content. You can use modules to prevent the submission of such content.'),
    'options' => array(
      array(
        'name' => 'CAPTCHA',
        'href' => 'http://drupal.org/project/captcha',
      ),
    ),
  );
  return theme('security_review_help_options', array('element' => $element));
}

function theme_security_review_help_options($element) {
  $output .= '<div class="sec-rev-help-option">';
  $output .= l($element['problem'], 'admin/reports/security-review/help', array('fragment' => $element['type'], 'attributes' => array('class' => 'sec-rev-help-dyn')));
  $output .= '<div class="sec-rev-help-content">';
  $output .= '<p>' . $element['description'] . '</p>';
  foreach ($element['options'] as $option) {
    $items[] = l($option['name'], $option['href']);
  }
  $output .= theme('item_list', array('items' => $items));
  $output .= '</div>';
  $output .= '</div>';
  return $output;
}

function security_review_check_file_perms_help($result = NULL) {
  $element['title'] =  t("Web server file system permissions");
  $element['descriptions'][] = t("It is dangerous to allow the web server to write to files inside the document root of your server. Doing so would allow Drupal to write files that could then be executed. An attacker might use such a vulnerability to take control of your site. An exception is the files directory which Drupal needs permission to write to in order to provide features like file attachments.");
  $element['descriptions'][] = t("In addition to inspecting files, this test attempts to create and write to files. Look in your security_review module directory on the server for files named file_write_test.YYYYMMDDHHMMSS and for a file called IGNOREME.txt which gets a timestamp appended to it if it is writeable.</p>");
  $element['descriptions'][] = t("<a href='!link'>Read more about file system permissions in the handbooks.</a>", array('!link' => url('http://drupal.org/node/244924')));

  $last_check = security_review_get_last_check('security_review', 'file_perms');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    if (is_null($result)) {
      $result = security_review_check_file_perms();
    }
    $element['findings']['descriptions'][] = t('It is recommended that the following files or directories be corrected.');
    foreach ($result['value'] as $file) {
      $element['findings']['items'][] = array(
        'safe' => check_plain($file),
        'raw' => $file,
      );
    }
  }
  return $element;
}

function security_review_check_input_formats_help($result = NULL) {
  $element['title'] = t('Allowed HTML tags in text formats');
  $element['descriptions'][] = t("Certain HTML tags can allow an attacker to take control of your site. Drupal's input format system makes use of a set filters to run on incoming text. The 'HTML Filter' strips out harmful tags and Javascript events and should be used on all formats accessible by untrusted users.");
  $element['descriptions'][] = t("<a href='!link'>Read more about Drupal's input formats in the handbooks.</a>", array('!link' => url('http://drupal.org/node/224921')));
  $last_check = security_review_get_last_check('security_review', 'input_formats');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    if (is_null($result)) {
      $result = security_review_check_input_formats();
    }
    if (!empty($result['value']['tags'])) {
      $element['findings']['descriptions'][] = t('<a href="!link">Review your text formats.</a>', array('!link' => url('admin/config/content/formats')));
      $element['findings']['descriptions'][] = t('It is recommended you remove the following tags from roles accessible by untrusted users.');
      foreach ($result['value']['tags'] as $tag) {
        $element['findings']['items'][] = array(
          'safe' => $tag, // Tag doesn't need filtering cause it's not user-defined.
          'raw' => $tag,
        );
      }
    }
    elseif (!empty($result['value']['formats'])) {
      $element['findings']['descriptions'][] = t('The following formats are usable by untrusted roles and do not filter or escape allowed HTML tags.');
      foreach ($result['value']['formats'] as $id => $format) {
        $element['findings']['items'][] = array(
          'html' => l($format->name, 'admin/config/content/formats/' . $format->format),
          'safe' => check_plain($format->name),
          'raw' => $format->name,
        );
      }
    }
  }
  return $element;
}

function security_review_check_php_filter_help($result = NULL) {
  $element['title'] = t('PHP Input Format');
  $element['descriptions'][] = t("Drupal's PHP Text Format allows for the interpretation and execution of PHP code via user-supplied input. Because this input runs in the context of Drupal itself it has access to everything Drupal does.");
  $last_check = security_review_get_last_check('security_review', 'untrusted_php');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    if (is_null($result)) {
      $result = security_review_check_php_filter();
    }
    if (!empty($result['value']['formats'])) {
      $element['findings']['descriptions'][] = t('The following formats are usable by untrusted roles and allow use of the PHP evaluator. You should edit the format to remove PHP use.');
      foreach ($result['value']['formats'] as $id => $format) {
        $element['findings']['items'][] = array(
          'html' => l($format->name, 'admin/config/content/formats/' . $format->format),
          'safe' => check_plain($format->name),
          'raw' => $format->name,
        );
      }
    }
  }
  return $element;
}

function security_review_check_error_reporting_help($result = NULL) {
  $element['title'] = t('Error reporting');
  $element['descriptions'][] = t('As a form of hardening your site you should avoid information disclosure. Drupal by default prints errors to the screen and writes them to the log. Error messages disclose the full path to the file where the error occured.');
  $last_check = security_review_get_last_check('security_review', 'error_reporting');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    if (is_null($result)) {
      $result = security_review_check_error_reporting();
    }
    $element['findings']['descriptions'][] = t('You have error reporting set to both the screen and the log.');
    $element['findings']['descriptions'][] = t('<a href="!link">Alter error reporting settings.</a>', array('!link' => url('admin/config/development/logging')));
  }
  return $element;
}

function security_review_check_private_files_help($result = NULL) {
  $element['title'] = t('Private files');
  $element['descriptions'][] = t("If you have Drupal's private files feature enabled you should move the files directory outside of the web server's document root. While Drupal will control serving files when requested by way of content if a user knows the actual system path they can circumvent Drupal's private files feature. You can protect against this by specifying a files directory outside of the webserver root.");
  $last_check = security_review_get_last_check('security_review', 'private_files');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    $element['findings']['descriptions'][] = t('Your files directory is not outside of the server root.');
    $element['findings']['descriptions'][] = t('<a href="!link">Edit the files directory path.</a>', array('!link' => url('admin/config/media/file-system')));
  }
  return $element;
}

function security_review_check_query_errors_help($result = NULL) {
  $element['title'] = t('Abundant query errors from the same IP');
  $element['descriptions'][] = t("Database errors triggered from the same IP may be an artifact of a malicious user attempting to probe the system for weaknesses like SQL injection or information disclosure.");
  $last_check = security_review_get_last_check('security_review', 'query_errors');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    $element['findings']['descriptions'][] = t('The following IPs were observed with an abundance of query errors.');
    if (is_null($result)) {
      $result = security_review_check_query_errors();
    }
    foreach ($result['value'] as $ip) {
      $element['findings']['items'][] = array(
        'safe' => check_plain($ip),
        'raw' => $ip,
      );
    }
  }
  return $element;
}

function security_review_check_failed_logins_help($results = NULL) {
  $element['title'] = t('Abundant failed logins from the same IP');
  $element['descriptions'][] = t("Failed login attempts from the same IP may be an artifact of a malicous user attempting to brute-force their way onto your site as an authenticated user to carry out nefarious deeds. ");

  $last_check = security_review_get_last_check('security_review', 'failed_logins');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    $element['findings']['descriptions'][] = t('The following IPs were observed with an abundanced of failed login attempts.');
    if (is_null($results)) {
      $results = security_review_check_failed_logins();
    }
    foreach ($results['value'] as $ip) {
      $element['findings']['items'][] = array(
        'safe' => check_plain($ip),
        'raw' => $ip,
      );
    }
  }
  return $element;
}

function security_review_check_admin_permissions_help($results = NULL) {
  $element['title'] = t('Admin and trusted Drupal permissions');
  $element['descriptions'][] = t("Drupal's permission system is extensive and allows for varying degrees of control. Certain permissions would allow a user total control, or the ability to escalate their control, over your site and should only be granted to trusted users.");
  $element['descriptions'][] = t('<a href="!link">Read more about trusted vs. untrusted roles and permissions on DrupalScout.com.</a>', array('!link' => url('http://drupalscout.com/knowledge-base/importance-user-roles-and-permissions-site-security')));
  $last_check = security_review_get_last_check('security_review', 'admin_permissions');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    if (is_null($results)) {
      $results = security_review_check_admin_permissions();
    }
    $roles = user_roles();
    $element['findings']['descriptions'][] = t('You have granted untrusted roles the following permissions that you should revoke.');
    foreach ($results['value'] as $rid => $permissions) {
      $permissions = implode(', ', $permissions);
      $html = t('<a href="!link">@name</a> has %permissions', array('!link' => url('admin/people/permissions/' . $rid), '@name' => $roles[$rid], '%permissions' => $permissions));
      $safe = t('@name has %permissions', array('@name' => $roles[$rid], $permissions));
      $element['findings']['items'][] = array(
        'html' => $html,
        'safe' => $safe,
        'raw' => $roles[$rid] . ':' . $permissions,
      );
    }
  }
  return $element;
}

function security_review_check_field_help($results = NULL) {
  $element['title'] = t('Dangerous tags in content');
  $element['descriptions'][] = t("Script and PHP code in content does not align with Drupal best practices and may be a vulnerability if an untrusted user is allowed to edit such content. It is recommended you remove such contents.");

  $last_check = security_review_get_last_check('security_review', 'field');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    $element['findings']['descriptions'][] = t('The following items potentially have dangerous tags.  ');
    if (is_null($results)) {
      $results = security_review_check_field(); // Don't pass $last_check because timestamp is wrong now.
    }
    foreach ($results['value'] as $entity_type => $value) {
      $ids = array_keys($value);
      $entity_info = entity_get_info($entity_type);
      $id = $entity_info['entity keys']['id'];
      $label = $entity_info['entity keys']['label'];
      $uri_callback = $entity_info['uri callback'];
      // There is no access checking. We state that the use of this module should be granted to trusted users only.
      $entities = entity_load($entity_type, $ids);
      foreach ($entities as $entity) {
        $uri = '#';
        if (function_exists($uri_callback)) {
          $uri = $uri_callback($entity);
          $uri = url($uri['path'] . '/edit'); // @todo can this assumption be made?
        }
        $html = t('@description found in <a href="!link">@title</a>', array('@description' => $value[$entity->{$id}], '!link' => $uri, '@title' => $entity->{$label}));
        $element['findings']['items'][] = array(
          'html' => $html,
          'safe' => t('@description in !url', array('@description' => $value[$entity->{$id}], '!url' => $uri)),
          'raw' => $value[$entity->{$id}] . ':' . $uri,
        );
      }
    }
    //$element['findings']['pager'] = theme('pager', array('tags' => NULL));
  }

  return $element;
}

function security_review_check_upload_extensions_help($results = NULL) {
  $element['title'] = t('Allowed upload extensions');
  $element['descriptions'][] = t("File and image fields allow for uploaded files. Some extensions are considered dangerous because the files can be evaluated and then executued in the browser. A malicious user could use this opening to gain control of your site.");
  $last_check = security_review_get_last_check('security_review', 'upload_extensions');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    if (is_null($results)) {
      $result = security_review_check_upload_extensions();
    }

    $element['findings']['descriptions'][] = t('The following extensions are considered unsafe and should be removed or limited from use. Or, be sure you are not granting untrusted users the ability to upload files.');
    foreach ($result['value'] as $field_name => $extensions) {
      foreach ($extensions as $extension) {
        $element['findings']['items'][] = array(
          'raw' => $extension,
          'safe' => check_plain($extension),
        );
      }
    }
  }
  return $element;
}

function security_review_check_name_passwords_help($results = NULL) {
  $element['title'] = t('Username as password');
  $element['descriptions'][] = t("Users with elevated access on the site (trusted users) who have a their account password the same as their username. It is recommended you enforce a password strength policy to avoid an attacker easily gaining access to your site.");

  $last_check = security_review_get_last_check('security_review', 'name_passwords');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  elseif ($last_check['result'] == '0') {
    $element['findings']['descriptions'][] = t('The following users have extremely weak passwords. The links go to the edit page.');
    if (is_null($results)) {
      $results = security_review_check_name_passwords(); // Don't pass $last_check because timestamp is wrong now.
    }
    foreach ($results['value'] as $uid => $name) {
      $element['findings']['items'][] = array(
        'html' => l($name, 'user/' . $uid . '/edit'),
        'safe' => check_plain($name),
        'raw' => $name,
      );
    }
    $element['findings']['pager'] = theme('pager', NULL, 20);
  }

  return $element;
}


// From here down is custom stuff added by Justin

function security_review_check_uid_1_blocked_help($result = NULL) {
	$element['title'] = t('Administrative account blocked.');
	$element['descriptions'][] = t('The administrative account (uid 1) is ' .
		'commonly targeted by attackers because this account has superuser ' .
		'privileges which cannot be blocked or limited.  Attacks that do things ' .
		'like change the administrator password, or even brute force or social ' .
		'engineering attacks could compromise the administrator password.  Because ' .
		'the administrative account has such wide privileges it is a good idea to ' .
		'create a role for administrators and explicitly create these less privileged ' .
		'accounts.  The administrative account can be unblocked by users with the ' .
		'"administer users" permission if you need to use the account at a later ' .
		'time.');
	if (is_null($result)) {
		$result = security_review_check_uid_1_blocked();
		}
	if ($result['result'] === FALSE) {
		$element['findings']['descriptions'][] = t('The administrative account is ' .
				'not blocked.');
		$element['findings']['descriptions'][] = t('<a href="!link">Disable the ' .
				'administrative account.</a>', array('!link' => url('user/1/edit')));
	}
	return $element;
}

function security_review_check_module_enabled_help($title, $descriptions, $module, $fail_descs) {
  $element['title'] = t($title);
  $element['descriptions'][] = t($descriptions);
  $last_check = security_review_get_last_check('security_review', strtolower($module) . '_module');
  if ($last_check['skip'] == '1') {
    $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
  }
  if ($last_check['result'] == '0') {	
    foreach($fail_descs as $findings) {
  	  $element['findings']['descriptions'][] = t($findings);
    }
  }
  $element['recommendation'] = TRUE;
  return $element;
}

function security_review_check_syslog_module_help($result = NULL) {
  $title = 'Syslog module';
  $descriptions = "Drupal includes a valuable Syslog module as " .
  		"part of core.  Enabling this module allows the operating system to manage " .
  		"Drupal logs (including intrusion and error detection capabilities).  This " .
  		"feature also allows for easier troubleshooting of Drupal.  Because of these " .
  		"capabilities, enabling the syslog module can greatly increase the security " .
  		"of your Drupal site.";
  $module = 'syslog';
  $fail_descs = array('The Syslog module is not enabled.',
    t('<a href="!link">Enable the Syslog module.</a>', array('!link' => url('admin/modules')))
  );
  return security_review_check_module_enabled_help($title, $descriptions, $module, $fail_descs);
}

function security_review_check_password_policy_module_help($result = NULL) {
  $title = 'Password Policy module';
  $descriptions = "Poor password choice is the leading cause of " .
  		"brute force attack (password guessing) success.  Choosing a strong password " .
  		"is essential for site security.  The Drupal Password Policy module " .
  		"(<a href='http://drupal.org/project/password_policy'>" .
  		"http://drupal.org/project/password_policy</a>) allows sites to force users " .
  		"to choose strong passwords.";
  $module = 'password_policy';
  $fail_descs = array(
	'The password policy module is not enabled.',
    t('<a href="!link">Enable the password strength module.</a>', array('!link' => url('admin/modules')))
  );
  return security_review_check_module_enabled_help($title, $descriptions, $module, $fail_descs);
}

function security_review_check_login_security_module_help($result = NULL) {
  $title = 'Login security module';
  $descriptions = "WASC-21 Anti-Automation " .
  		"(<a href='http://projects.webappsec.org/w/page/13246938/Insufficient Anti-automation'>" .
  		"http://projects.webappsec.org/w/page/13246938/Insufficient%20Anti-automation</a>) " .
  		"is described as failure to prevent an automated attack (such as brute force " .
  		"password guessing) against a web application.  The Drupal login security module " .
  		"(<a href='http://drupal.org/project/login_security'>http://drupal.org/project/login_security</a>) " .
  		"allows sites to prevent brute force attacks by rate limiting logins, alerting " .
  		"administrators of attacks, and banning offending IP's or users.  The module is a " .
  		"simple and effective way to prevent password guessing attacks.";
  $module = 'login_security';
  
  $fail_descs = array(
	'The login security module is not enabled.',
    t('<a href="!link">Enable the login security module.</a>', array('!link' => url('admin/modules')))
  );
  
  return security_review_check_module_enabled_help($title, $descriptions, $module, $fail_descs);
}

function security_review_check_recent_cron_help($result = NULL) {
	$element['title'] = t('Cron has been run recently.');
	$element['descriptions'][] = t('The Drupal cron task ensures that regular ' .
		'database maintenance has occurred.  Often security related jobs are ' .
		'tied to cron.  Ensuring cron runs on a regular basis (for instance in ' .
		'the last day or so) helps improve site stability, a building block for ' .
		'good security.');
	if (is_null($result)) {
		$result = security_review_check_recent_cron();
	}
	if ($result['result'] === FALSE) {
		$element['findings']['descriptions'][] = t('Cron has not been run in the ' .
				'last 24 hours.');
		$element['findings']['descriptions'][] = t('<a href="!link">View details ' .
				'about cron maintenance tasks.</a>', 
				array('!link' => url('admin/reports/status')));
	}
	return $element;
}

function security_review_check_anon_content_help($result = NULL) {
	$element['title'] = t('Anonymous owned content.');
	$element['descriptions'][] = t('Nodes owned by user ID (UID) 0 are owned by the built ' .
			'in <em>anonymous</em> account which is used by anyone who doesn\'t authenticate.  ' .
			'This could indicate orphaned conent in situations where user accounts for ' .
			'the noce creator was later deleted.  Alternatively an automated ' .
			'process could create orphaned nodes.');
	$element['descriptions'][] = t('Unfortunately it is easy for <a href="?q=admin/people/permissions">' .
			'permissions</a> to be misapplied to these nodes so that the "owner" of the node ' .
			'could edit content.  This would allow any unauthenticated user the ability ' .
			'to edit the node and potentially place malicious content on the site.  It is ' .
			'important to take ownership of all anonymously owned nodes.');
	if (is_null($result)) {
		$result = security_review_check_anon_content();
		}
	if ($result['result'] === FALSE) {
		$element['findings']['descriptions'][] = t('The following content is owned by UID 0:');
		foreach ($result['value'] as $result) {
			$nid = intval(key($result));
			$title = check_plain($result[$nid]);
			$element['findings']['descriptions'][] = t('<a href="!link">Edit node ' .
				$title . '.</a>', array('!link' => url('node/' . $nid . '/edit')));
		}
		
	}
	return $element;
}



function security_review_check_password_policies_help($result = NULL) {
	$element['title'] = t('No defined policies for passwords.');
	$element['descriptions'][] = t('The Password Policy module allows administrators to ' .
			'set rules around password strength, composition, and expiration.  With no defined ' .
			'policies the module is inert and useless.  Setting some minimum password ' .
			'policies helps to protect users.');
	$last_check = security_review_get_last_check('security_review', 'password_policies');
    if ($last_check['skip'] == '1') {
      $element['findings']['descriptions'][] = _security_review_check_skipped($last_check);
    }
    elseif ($last_check['result'] == '0') {
	  $element['descriptions'][] = 'Set up a new <a href="?q=admin/config/people/password_policy/add">' .
			'password policy</a>.';
    }
	return $element;
}