<?php
/**
 * @file
 * Admin page callbacks for the flood_unblock module.
 */

/**
 * Form constructor: lists flood entries so they can be cleared.
 *
 * @see flood_unblock_settings_validate()
 * @see flood_unblock_settings_submit()
 */
function flood_unblock_settings($form, &$form_state) {
  $entries = flood_unblock_get_entries();

  $form['help'] = array(
    '#markup' => '<p>' . t('People are blocked after too many failed logins or password reset requests, either for one account or from one IP address. Blocks lift on their own once the attempts age out; clear them here to lift a block now. The limits are set on the <a href="@url">account settings</a> page.', array('@url' => url('admin/config/people/login'))) . '</p>',
  );

  $options = array();
  $map = array();
  foreach ($entries as $key => $entry) {
    $options[$key] = array(
      'blocked' => $entry['blocked'] ? t('Blocked') : '',
      'type' => check_plain($entry['label']),
      'count' => t('@count of @limit', array('@count' => $entry['count'], '@limit' => $entry['limit'])),
      'who' => $entry['who'],
      'last' => format_date($entry['last'], 'short'),
      'lifts' => $entry['lifts'] ? format_date($entry['lifts'], 'short') : '',
    );
    $map[$key] = array(
      'event' => $entry['event'],
      'identifier' => $entry['identifier'],
      'label' => $entry['label'],
      'who' => $entry['who_plain'],
    );
  }

  $form['table'] = array(
    '#type' => 'tableselect',
    '#header' => array(
      'blocked' => t('Status'),
      'type' => t('Type'),
      'count' => t('Attempts'),
      'who' => t('Account / IP address'),
      'last' => t('Last attempt'),
      'lifts' => t('Block lifts'),
    ),
    '#options' => $options,
    '#empty' => t('There are no failed logins or password reset requests at this time.'),
  );

  // Kept server-side, so only entries listed on the form can be cleared.
  $form['entries'] = array(
    '#type' => 'value',
    '#value' => $map,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Clear selected'),
    '#disabled' => empty($entries),
  );

  $form['recent'] = flood_unblock_recent_clears();

  return $form;
}

/**
 * Builds a table of the most recent unblocks, read from the database log.
 *
 * Clearing deletes the flood rows, so the log is the only record of who
 * unblocked whom and when.
 *
 * @param int $limit
 *   How many entries to show.
 *
 * @return array
 *   A render array, or an empty array when Database logging is not enabled.
 */
function flood_unblock_recent_clears($limit = 10) {
  if (!module_exists('dblog')) {
    return array();
  }
  module_load_include('inc', 'dblog', 'dblog.admin');

  $events = db_select('watchdog', 'w')
    ->fields('w', array('wid', 'uid', 'message', 'variables', 'timestamp'))
    ->condition('w.type', 'flood_unblock')
    ->orderBy('w.wid', 'DESC')
    ->range(0, $limit)
    ->execute()
    ->fetchAll();

  $uids = array();
  foreach ($events as $event) {
    $uids[$event->uid] = $event->uid;
  }
  $accounts = $uids ? user_load_multiple($uids) : array();

  $rows = array();
  foreach ($events as $event) {
    $rows[] = array(
      format_date($event->timestamp, 'short'),
      isset($accounts[$event->uid]) ? theme('username', array('account' => $accounts[$event->uid])) : t('Unknown'),
      // Core's formatter: translates with the stored variables, then
      // filter_xss_admin().
      dblog_format_message($event),
    );
  }

  return array(
    '#type' => 'fieldset',
    '#title' => t('Recently unblocked'),
    'table' => array(
      '#theme' => 'table',
      '#header' => array(t('When'), t('By'), t('What')),
      '#rows' => $rows,
      '#empty' => t('Nothing has been unblocked yet.'),
    ),
  );
}

/**
 * Form validation handler for flood_unblock_settings().
 */
function flood_unblock_settings_validate($form, &$form_state) {
  if (!array_filter($form_state['values']['table'])) {
    form_set_error('table', t('Please select at least one entry.'));
  }
}

/**
 * Form submission handler for flood_unblock_settings().
 *
 * Clears every flood event recorded for each selected event and identifier.
 */
function flood_unblock_settings_submit($form, &$form_state) {
  $map = $form_state['values']['entries'];
  $cleared = 0;
  foreach (array_filter($form_state['values']['table']) as $key) {
    if (isset($map[$key])) {
      $item = $map[$key];
      flood_clear_event($item['event'], $item['identifier']);
      watchdog('flood_unblock', 'Unblocked %who: %type.', array('%who' => $item['who'], '%type' => $item['label']));
      $cleared++;
    }
  }
  backdrop_set_message(format_plural($cleared, 'Cleared 1 flood entry.', 'Cleared @count flood entries.'));
}
