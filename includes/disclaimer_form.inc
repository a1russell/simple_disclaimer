<?php


/**
 * Disclaimer form.
 */
function disclaimer_form($form, &$form_state) {
  // Enable form theming
  $form['#theme'] = 'disclaimer_form';
  $form['ack'] = array(
    '#type' => 'checkbox',
    '#title' => t('I have read and acknowledge this disclaimer.'),
    '#default_value' => 0,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  unset($_SESSION['simple_disclaimer_redirect_count']);
  return $form;
}


/**
 * Validation for disclaimer form.
 */
function disclaimer_form_validate($form, &$form_state) {
  // Disclaimer must be acknowledged.
  if ($form_state['values']['ack'] != 1) {
    form_set_error('ack', t('You must acknowledge the disclaimer in order to proceed.'));
  }
}


/**
 * Disclaimer form submission.
 */
function disclaimer_form_submit($form, &$form_state) {
  global $user;
  $uid = $user->uid;
  $ack = $form_state['values']['ack'];

  // Save acknowledgment
  if ($uid != 0) {
    $edit['data']['simple_disclaimer_ack'] = $ack;
    user_save($user, $edit);
  }
}


/**
 * Theme function for the dislaimer page.
 */
function theme_disclaimer_form($variables) {
  $output = '<div class="disclaimer-ack">';
  $disclaimer = array();

  // Populate disclaimer array with disclaimer message
  $disclaimer_query_str = <<< 'EOD'
    SELECT DISTINCT d.disclaimer
    FROM {disclaimer} AS d
    WHERE d.disclaimer != '';
EOD;
  $result= db_query($disclaimer_query_str);
  foreach ($result as $record) {
    $disclaimer[] = $record->disclaimer;
  }

  $output .=  "<p>{$disclaimer[0]}</p>";

  // Isolate the form definition form the $variables array
  $form = $variables['form'];

  // Form elements are rendered with drupal_render()
  $output .= drupal_render($form['ack']);

  // Pass the remaining form elements through drupal_render_children()
  $output .= drupal_render_children($form);

  $output .= '</div>';

  return $output;
}
