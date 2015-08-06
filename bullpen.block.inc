<?php
/**
 * @file bullpen.block.inc
 *
 * Block HOOK implementations for the Bullpen module. This will provide one
 * block with a WYSIWYG editor so users can create a call to action block. to place
 * throughout the site for users with permissions that will link them to the upload form.
 *
 * Created by PhpStorm.
 * User: rrf011
 * Date: 8/4/2015
 * Time: 3:36 PM
 */

/**
 * Implements hook_block_info().
 */
function bullpen_block_info() {
  $blocks = array();

  $blocks['bullpen'] = array(
    'info' => t('Upload Bullpen Link'),
    'cache' => DRUPAL_CACHE_PER_ROLE,
  );

  return $blocks;
}

/**
 * Implements hook_block_configure().
 */
function bullpen_block_configure($delta = '') {
  $form = array();
  switch ( $delta ) {
    case 'bullpen':

      $defaults = array(
        'value' => '',
        'format' => filter_default_format(),
      );
      $default_value = variable_get('bullpen_block_text', $defaults);

      $form['bullpen_block_text'] = array(
        '#type' => 'text_format',
        '#title' => t('Body'),
        '#default_value' => $default_value['value'],
        '#format' => $default_value['format'],
        '#description' => t('Enter the HTML to create a call to action (CTA) block. You may use the token [bullpen:upload_url] to insert the link where users will be able to upload files.')
      );
      break;
  }
  return $form;
}

/**
 * Implements hook_block_save().
 */
function bullpen_block_save($delta = '', $edit = array()) {
  switch ($delta) {
    case 'bullpen':
      variable_set('bullpen_block_text', $edit['bullpen_block_text']);
      break;
  }

}

/**
 * Implements hook_block_view().
 */
function bullpen_block_view($delta = '') {
  global $user;
  $block = array();

  switch ($delta) {
    case 'bullpen':
      if ( user_access('upload files to bullpen') ) {
        $content = variable_get('bullpen_block_text', '[bullpen:upload_url]');
        $block['subject'] = '';
        $block['content'] = check_markup($content['value'], $content['format']);
      }
      break;
  }
  return $block;
}