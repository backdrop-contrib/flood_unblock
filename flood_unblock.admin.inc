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
    $map[$key] = array($entry['event'], $entry['identifier']);
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

  return $form;
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
      list($event, $identifier) = $map[$key];
      flood_clear_event($event, $identifier);
      watchdog('flood_unblock', 'Cleared flood event %event for %identifier.', array('%event' => $event, '%identifier' => $identifier));
      $cleared++;
    }
  }
  backdrop_set_message(format_plural($cleared, 'Cleared 1 flood entry.', 'Cleared @count flood entries.'));
}
