<?php


/**
 * Disclaimer configuration form.
 */
function simple_disclaimer_form($form, &$form_state) {
  $roles = array();
  $user_roles = array();
  $disclaimer_user_roles = array();
  $previously_submitted_disclaimer = array();
  $previously_submitted_roles = array();
  $message = ''; // Default value for the message field.

  $disclaimer_query_str = <<< 'EOD'
    SELECT d.id, d.rid, d.role_name, d.disclaimer
    FROM {disclaimer} AS d
    WHERE d.disclaimer != ''
    ORDER BY d.role_name ASC;
EOD;
  $result = db_query($disclaimer_query_str);

  foreach ($result as $record) {
    $previously_submitted_disclaimer[$record->id] = $record->disclaimer;
  }
  foreach ($previously_submitted_disclaimer as $key => $value) {
    $message=$value;
  }

  // Default value for the role field in the form
  $result1 = db_query($disclaimer_query_str);
  foreach ($result1 as $record) {
    $previously_submitted_roles[$record->rid] = $record->rid;
  }

  /* Count the number of records in the disclaimer table. If the records are present then
     we don't need run the $role_query_str, which is used to insert record in the disclaimer table. */
  $disclaimer_role_query_str = <<< 'EOD'
    SELECT d.rid
    FROM {disclaimer} as d;
EOD;
  $result = db_query($disclaimer_role_query_str);

  foreach ($result as $record) {
    $disclaimer_user_roles[$record->rid] = $record->rid;
  }

  // Populate roles array with all roles defined in the system except "anonymous user" role.
  $role_query_str = <<< 'EOD'
    SELECT r.rid, r.name
    FROM {role} AS r
    WHERE r.rid != 1
    ORDER BY r.name ASC;
EOD;
  $result = db_query($role_query_str);
  foreach ($result as $record) {
    $roles[$record->rid] = $record->name;
  }

  if (count($disclaimer_user_roles) != count($roles)) {
    // Delete existing records from the disclaimer table in the database
    db_query('DELETE FROM {disclaimer}');
    // Stores the roles in the disclaimer table
    $query = db_insert('disclaimer')->fields(array('rid', 'role_name'));
    foreach ($roles as $rid => $name) {
      $query->values(array("$rid", "$name"));
    }
    $query->execute();
  }

  // Populate user_roles array to display as options on the configuration page.
  $user_roles_query_str = <<< 'EOD'
    SELECT d.rid, d.role_name
    FROM {disclaimer} AS d
    ORDER BY d.role_name ASC;
EOD;
  $result= db_query($user_roles_query_str);
  foreach ($result as $record) {
    $user_roles[$record->rid] = $record->role_name;
  }

  $form['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles that should see the disclaimer:'),
    '#options' => $user_roles,
    '#multiple' => TRUE,
    '#description' => t('If no role is selected then all users will have to acknowledge the disclaimer.'),
    '#default_value' => $previously_submitted_roles,
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#cols' => 60,
    '#rows' => 5,
    '#title' => t('Disclaimer message:'),
    '#required' => TRUE,
    '#default_value' => $message,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}


/**
 * Disclaimer configuration form submission.
 */
function simple_disclaimer_form_submit($form, &$form_state) {
  $message = $form_state['values']['message'];
  $user_roles = $form_state['values']['roles'];
  $i = 0;

  // Sanitize disclaimer.
  $disclaimer = filter_xss_admin($message);

  // Count element in an array
  $length = count($user_roles);

  foreach ($user_roles as $rid => $value) {
    if ($value == 0) {
      $i++;
      /* If element count in an array is equal to i that means user has not selected any role.
         So all the roles will see the disclaimer. */
      if ($length == $i) {
        foreach ($user_roles as $rid => $value) {
          // Set disclaimer message in the disclaimer table for all roles
          $num_updated = db_update('disclaimer')->fields(array(
            'disclaimer' => $disclaimer,
          ))->condition('rid', $rid, '=')->execute();
        }
      }

      // Set disclaimer message = '' in the disclaimer table for the unselected role.
      else {
        $num_updated = db_update('disclaimer')->fields(array(
          'disclaimer' => '',
        ))->condition('rid', $rid, '=')->execute();
      }
    } else {
      // Set disclaimer message in the disclaimer table for the selected users
      $num_updated = db_update('disclaimer')->fields(array(
        'disclaimer' => $disclaimer,
      ))->condition('rid', $value, '=')->execute();
    }
  }

  drupal_set_message("The configuration options have been saved.");
}
