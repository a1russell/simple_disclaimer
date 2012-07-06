<?php


/**
 * Disclaimer configuration form.
 */
function simple_disclaimer_form($form, &$form_state) {
  $message = variable_get('simple_disclaimer', '');

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
  $disclaimer = filter_xss_admin($message);
  variable_set('simple_disclaimer', $disclaimer);
  drupal_set_message("The configuration options have been saved.");
}
