<?php
/**
 * Created by PhpStorm.
 * User: rrf011
 * Date: 8/4/2015
 * Time: 9:58 AM
 */

function bullpen_upload_form($form, &$form_state) {
  $form = array();

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Node'),
    '#description' => t('Begin typing the name of the node that you wish to submit files for. The autocomplete field will display nodes who\'s title is similiar and allow you to select from the list.'),
    '#default_value' => '',
    '#autocomplete_path' => 'bullpen_autocomplete',
    '#required' => TRUE,
  );

  $form['bullpen'] = array(
    '#type' => 'plupload',
    '#title' => t('Upload files'),
    '#description' => t('This multi-upload widget uses Plupload library.'),
    '#upload_validators' => array(
      'file_validate_extensions' => array('jpg jpeg png'),
    ),
    '#plupload_settings' => array(
      'runtimes' => 'html5',
      'chunk_size' => '1mb',
    ),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

function bullpen_upload_form_submit($form, &$form_state) {
  global $user;
  $upload_request = array(
    'entity_id' => _bullpen_get_entity_id_from_title($form_state['values']['title']),
    'description' => '',
    'uid' => $user->uid,
    'created' => REQUEST_TIME,
    'changed' => REQUEST_TIME,
  );
  $rid = db_insert('bullpen_request')->fields($upload_request)->execute();

  drupal_set_message(t('Request @rid successful.', array('@rid' => $rid)), 'status');
}

function bullpen_upload_form_validate($form, &$form_state) {
  if (! _bullpen_get_entity_id_from_title($form_state['values']['title']) ) {
    form_set_error('entity_id', t('Invalid node &mdash; node @title does not exist!', array('@title' => $form_state['values']['title'])));
  }
}

function bullpen_approval_form($form, &$form_state, $node) {
  $form = array();

  // Capture our node as a property so we can use it in the submit handler.
  $form['#node'] = $node;

  return $form;
}

function bullpen_approval_form_submit($form, &$form_state) {

}

function bullpen_approval_form_validate($form, &$form_state) {

}

function bullpen_item_approval_form($form, $form_state, $bullpen_item) {

}

function bullpen_item_approval_form_submit($form, $form_state) {

}

/**
 * Menu callback to get published node titles for files to be attached to. Adapted
 * from the @see user_autocomplete() in the user module.
 * @param string $name
 */
function bullpen_autocomplete($string='') {
  // @todo load allowed types from settings.
  $types = array();
  $matches = array();
  if ($string) {
    $query = db_select('node', 'n')
      ->fields('n', array('title'))
      ->condition('n.title', db_like($string) . '%', 'LIKE')
      ->condition('n.status', 1, '=');

    if ( count($types) ) {
      $query->condition('n.type', $types, 'IN');
    }

    $result = $query->range(0, 10)->execute();

    foreach ($result as $node) {
      $matches[$node->title] = check_plain($node->title);
    }
  }

  drupal_json_output($matches);
}