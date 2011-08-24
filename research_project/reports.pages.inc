<?php

/**
 * @file
 * Administrative page callbacks for the dblog module.
 */

/**
 * dblog module settings form.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function dblog_admin_settings() {
  $form['dblog_row_limit'] = array(
    '#type' => 'select',
    '#title' => t('Discard log entries above the following row limit'),
    '#default_value' => variable_get('dblog_row_limit', 1000),
    '#options' => drupal_map_assoc(array(100, 1000, 10000, 100000, 1000000)),
    '#description' => t('The maximum number of rows to keep in the database log. Older entries will be automatically discarded. (Requires a correctly configured <a href="@cron">cron maintenance task</a>.)', array('@cron' => url('admin/reports/status')))
  );

  return system_settings_form($form);
}

/**
 * Menu callback; displays a listing of log messages.
 */
function dblog_overview($pid) {
	global $user;
 // $filter = dblog_build_filter_query();
  $rows = array();
  $icons = array(
    WATCHDOG_DEBUG    => '',
    WATCHDOG_INFO     => '',
    WATCHDOG_NOTICE   => '',
    WATCHDOG_WARNING  => theme('image', 'misc/watchdog-warning.png', t('warning'), t('warning')),
    WATCHDOG_ERROR    => theme('image', 'misc/watchdog-error.png', t('error'), t('error')),
    WATCHDOG_CRITICAL => theme('image', 'misc/watchdog-error.png', t('critical'), t('critical')),
    WATCHDOG_ALERT    => theme('image', 'misc/watchdog-error.png', t('alert'), t('alert')),
    WATCHDOG_EMERG    => theme('image', 'misc/watchdog-error.png', t('emergency'), t('emergency')),
  );
  $classes = array(
    WATCHDOG_DEBUG    => 'dblog-debug',
    WATCHDOG_INFO     => 'dblog-info',
    WATCHDOG_NOTICE   => 'dblog-notice',
    WATCHDOG_WARNING  => 'dblog-warning',
    WATCHDOG_ERROR    => 'dblog-error',
    WATCHDOG_CRITICAL => 'dblog-critical',
    WATCHDOG_ALERT    => 'dblog-alert',
    WATCHDOG_EMERG    => 'dblog-emerg',
  );

  $header = array(
    ' ',
    array('data' => t('Project'), 'field' => 'w.type'),
    array('data' => t('Date'), 'field' => 'w.wid', 'sort' => 'desc'),
    t('Message'),
    array('data' => t('From'), 'field' => 'u.name'),

  );

  $sql = "SELECT w.wid, w.uid, w.severity, w.type, w.timestamp, w.message, w.variables, w.link, u.name FROM {watchdog} w INNER JOIN {users} u ON w.uid = u.uid WHERE u.name='$user->name' and w.type = 'rproject_$pid'";
  $tablesort = tablesort_sql($header);
  $result = pager_query($sql . $tablesort, 10);
 

  while ($dblog = db_fetch_object($result)) {
    $rows[] = array('data' =>
      array(
        // Cells
        $icons[$dblog->severity],
        t($dblog->type),
        format_date($dblog->timestamp, 'small'),
        l(truncate_utf8(_dblog_format_message($dblog), 56, TRUE, TRUE), 'admin/reports/event/'. $dblog->wid, array('html' => TRUE)),
        theme('username', $dblog),
        $dblog->link,
      ),
      // Attributes for tr
      'class' => "dblog-". preg_replace('/[^a-z]/i', '-', $dblog->type) .' '. $classes[$dblog->severity]
    );
  }

  if (!$rows) {
    $rows[] = array(array('data' => t('No log messages available.'), 'colspan' => 6));
  }

  $output .= theme('table', $header, $rows, array('id' => 'admin-dblog'));
  $output .= theme('pager', NULL, 50, 0);

  return $output;
}
/**
 * Formats a log message for display.
 *
 * @param $dblog
 *   An object with at least the message and variables properties
 */
function _dblog_format_message($dblog) {
  // Legacy messages and user specified text
  if ($dblog->variables === 'N;') {
    return $dblog->message;
  }
  // Message to translate with injected variables
  else {
    return t($dblog->message, unserialize($dblog->variables));
  }
}
