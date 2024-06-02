<?php

/**
 * Provide shopping cart related Views integration.
 */

/**
 * Implements hook_views_data_alter().
 */
function commerce_custom_views_add_to_cart_views_data_alter(&$data) {
  // Use our custom handler.
  $data['views_entity_commerce_product']['add_to_cart_form']['field']['handler'] = 'commerce_custom_views_add_to_cart_handler_field_add_to_cart_form';
}
